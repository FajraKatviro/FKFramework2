#include "%ClassName%.%CppHeaderSuffix%"

@if "%Servant%" == "false"
FK_NO_SERVANT(%ClassName%)
@endif
@if "%Servant%" == "true"
struct %ClassName%::Servant{

};

void %ClassName%::servantInitialization(){

}

void %ClassName%::servantDeinitialization(){

}
@endif

FK_EVENTS(%ClassName%){
    //common.append("o_prop");
    //custom.append("u_prop");
    //events["e_"]=&%ClassName%::e_event;
    //events["a_"]=&%ClassName%::a_action;
}

void %ClassName%::customInitialization(){

}

void %ClassName%::customDeinitialization(){

}

//----------------------------------------

