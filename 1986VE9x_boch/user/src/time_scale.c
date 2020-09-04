#include "time_scale.h"

_Time_scale BochScaleStruct;

/*******************************************************************
                       ”становка шкалы времени
*******************************************************************/
void Clean_Time_Scale(_Time_scale* time_scale_struct){
    memset(time_scale_struct, 0, sizeof(time_scale_struct));
}

/*******************************************************************
                       »нкремент шкалы времени
*******************************************************************/
void Inc_Time_Scale(_Time_scale* time_scale_struct){
	_Time_scale* currentScale = time_scale_struct;
	
    currentScale->Seconds++; 
	if(currentScale->Seconds == 60){
        currentScale->Seconds = 0;
		currentScale->Minutes++;
		if(currentScale->Minutes == 60){
            currentScale->Minutes = 0; 
			currentScale->Hours++;
			if(currentScale->Hours == 24){
				currentScale->Hours = 0; 
				if(currentScale->Days == 30 && (currentScale->Months == 4 || currentScale->Months == 6 || currentScale->Months == 9 || currentScale->Months == 11)){
					currentScale->Days = 1;
					currentScale->Months++;
					return;
				}
				if(currentScale->Days == 28 && currentScale->Months == 2 && !(currentScale->Years % 4)){
					currentScale->Days++;
					return;
				}
				if(currentScale->Days == 29 && currentScale->Months == 2 && !(currentScale->Years % 4)){ // 50 
                    currentScale->Days = 1;
					currentScale->Months++;                                               
                    return;
				}
				if(currentScale->Days == 28 && currentScale->Months == 2 && (currentScale->Years % 4)){
					currentScale->Days = 1;
					currentScale->Months++;
					return;
				}
				currentScale->Days++;
				if(currentScale->Months == 12 && currentScale->Days == 32){	
					currentScale->Days = 1;
					currentScale->Months = 1;
					currentScale->Years++;
					return;
				}
				if(currentScale->Days == 32){
					currentScale->Days = 1;
					currentScale->Months++;
				}                                           
				return;
			}
		}
	}
}
