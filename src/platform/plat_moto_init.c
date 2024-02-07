
#include <loader.h>
#include <apps.h>
#include <mem.h>
#include <mme.h>
#include <uis.h>

/* Эта структура идентифицирует приложение. Передаётся в любой обработчик событий */
typedef struct
{ 
    APPLICATION_T           apt; /* Должно обязательно присутствовать, причём первым */

    /* Здесь кладутся прочие данные приложения */
    /* ... */

} APP_HELLOWORLD_T;

/* Состояния приложения */
typedef enum
{
    HW_STATE_ANY, /* ANY-state всегда первым */

    HW_STATE_INIT,
    HW_STATE_MAIN,

    HW_STATE_MAX /* Для удобства */
} HW_STATES_T;

UINT32 Register( char* file_uri,  char* param,  UINT32 reserve );

UINT32 HelloWorldStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 );
UINT32 HelloWorldExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type );

UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  APPLICATION_T *app );

/* Здесь мы будем хранить выданную нам "базу" ивентов */
UINT32              evcode_base;

/* Название приложения. Длина строки именно такая и никакая иначе */
const char app_name[APP_NAME_LEN] = "HelloWorld"; 

// Обработчики событий для каждого state-а

/* Обработчики событий для HW_STATE_ANY (используется в любом state) */
const EVENT_HANDLER_ENTRY_T any_state_handlers[] =
{  
    // GUI приложения должы ловить этот ивент
    { EV_REVOKE_TOKEN,              APP_HandleUITokenRevoked },
    { STATE_HANDLERS_END,           NULL           },
};

const EVENT_HANDLER_ENTRY_T init_state_handlers[] =
{
    // Это событие означает, что мы можем работать с UI
    { EV_GRANT_TOKEN,               HandleUITokenGranted },
    { STATE_HANDLERS_END,           NULL           },
};

const EVENT_HANDLER_ENTRY_T main_state_handlers[] =
{
    { EV_DIALOG_DONE,               HelloWorldExit },
    { STATE_HANDLERS_END,           NULL           },
};

/* Таблица соответствий обработчиков, состояний и функций входа-выхода из состояния.
   Порядок состояний такой же, как в enum-e */
static const STATE_HANDLERS_ENTRY_T state_handling_table[] =
{
    { HW_STATE_ANY,               // State
      NULL,                       // Обработчик входа в state
      NULL,                       // Обработчик выхода из state
      any_state_handlers          // Список обработчиков событий
    },

    { HW_STATE_INIT,
      NULL,
      NULL,
      init_state_handlers
    },
    
    { HW_STATE_MAIN,
      MainStateEnter,
      NULL,
      main_state_handlers
    }
};

/* Это entry для всех эльфов, должна быть объявлена именно так */
/* file_uri - путь к эльфу (аналогично argv[0]) */
/* param - параметры эльфа (аналогично остальным argv) */
/* reserve - выделенная база eventcode-ов (зарезервировано 64 значения)*/
UINT32 Register( char* file_uri,  char* param,  UINT32 reserve )
{
    UINT32 status = RESULT_OK;
    evcode_base = reserve;    // Сохраняем reserve
    

/* Регистрируем приложение */
    status = APP_Register( &evcode_base,            // На какое событие должно запускаться приложение
                           1,                       // Кол-во событий, на которое должно запускаться приложение
                           state_handling_table,    // Таблица состояний
                           HW_STATE_MAX,            // Количество состояний
                           (void*)HelloWorldStart );   // Функция, вызываемая при запуске
                   
    UtilLogStringData(" *** ELF *** HelloWorld: Register status %d", status);
    
    LdrStartApp(reserve);   // Запустить немедленно
    
    return 1;   // Пока не имеет значения, что возвращать
}


/* Функция вызываемая при старте приложения */
UINT32 HelloWorldStart( EVENT_STACK_T *ev_st,  REG_ID_T reg_id,  UINT32 param2 )//void *reg_hdl )
{
    APP_HELLOWORLD_T     *app = NULL;
    UINT32 status = RESULT_OK;
    
    UtilLogStringData(" *** ELF *** HelloWorld: HelloWorldStart");

    /* Проверяем, а вдруг уже запущено? */
    if( AFW_InquireRoutingStackByRegId( reg_id ) == RESULT_OK )
        return RESULT_FAIL; // Нам не нужен второй экземпляр

    // Инициализация для фоновых приложений
    app = (APP_HELLOWORLD_T*)APP_InitAppData( (void *)APP_HandleEvent, // Обработчик для приложений с GUI
                                              sizeof(APP_HELLOWORLD_T), // Размер структуры приложения
                                              reg_id,
                                              0, 1,
                                              1,
                                              1, 1, 0 );

    UtilLogStringData(" *** ELF *** HelloWorld: APP_InitAppData 0x%X", app);
    status = APP_Start( ev_st,
                        &app->apt,
                        HW_STATE_INIT, // Начальное состояние
                        state_handling_table,
                        HelloWorldExit,
                        app_name,
                        0 );

    UtilLogStringData(" *** ELF *** HelloWorld: APP_Start 0x%X", status);

    return RESULT_OK;
}


/* Функция выхода из приложения */
UINT32 HelloWorldExit( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    UINT32  status;

    /* Завершаем работу приложения */
    status = APP_Exit( ev_st, app, 0 );

    /* Выгружаем эльф */
    LdrUnloadELF(&Lib); // &Lib указывает на начало эльфа

    return status;
}

/* Обработчик входа в main state */
UINT32 MainStateEnter( EVENT_STACK_T *ev_st,  APPLICATION_T *app,  ENTER_STATE_TYPE_T type )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    SU_PORT_T               port = papp->port;
    CONTENT_T               content;
    UIS_DIALOG_T            dialog = 0;
	const WCHAR msg[] = L"Hello world!";

	if(type!=ENTER_STATE_ENTER) return RESULT_OK;
    
    UIS_MakeContentFromString("MCq0", &content, msg); //Создаём контент из строки
    
    dialog = UIS_CreateTransientNotice( &port, &content, NOTICE_TYPE_OK );
    
    UtilLogStringData(" *** ELF *** HelloWorld: MainStateEnter 0x%X", dialog);
    
    if(dialog == 0) return RESULT_FAIL;

    papp->dialog = dialog;

    return RESULT_OK;
}

UINT32 HandleUITokenGranted( EVENT_STACK_T *ev_st,  APPLICATION_T *app )
{
    APPLICATION_T           *papp = (APPLICATION_T*) app;
    UINT32                  status;

    // Вызываем дефолтный обработчик события
    status = APP_HandleUITokenGranted( ev_st, app );

    // Если всё хорошо, меняем текущий state
    if( (status == RESULT_OK) && (papp->token_status == 2) )
    {
        status = APP_UtilChangeState( HW_STATE_MAIN, ev_st, app );
    }

    return status;
}
  