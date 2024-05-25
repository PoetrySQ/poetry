/*
 * mppt.c
 *
 *  Created on: May 25, 2024
 *      Author: SSQ
 */


#include <mppt.h>
#include <PID.h>

mpptVariables mpptMain;

void mpptInit(mpptVariables *arg)
{
    arg->I_current = 0.0;
    arg->I_previous = 0.0;
    arg->P_current = 0.0;
    arg->P_previous = 0.0;
    arg->P_current = 0.0;
    arg->V_previous = 0.0;
    arg->V_current = 0.0;
    arg->dI = 0.0;
    arg->dV = 0.0;
    arg->Verr = 0.0;
    arg->step = 0.1;

    PID_init(0.0, 0.0, 0.0, 0.8, 0, &mpptPid);
}

//Perturbation and observation method
void mpptPno(mpptVariables *Pno, float Vpv, float Ipv)
{
	Pno->V_current = Vpv;
	Pno->I_current = Ipv;
    Pno->V_current += Pno->dV;
    if (Pno->P_current<Pno->P_previous)
    {
        /* code */
        if (Pno->V_current>Pno->V_previous)
        {
            /* code */
            Pno->Vref -= Pno->dV;
        }
        else
        {
            /* code */
            Pno->Vref += Pno->dV;
        }
    }
    else
    {
        /* code */
        if (Pno->V_current>Pno->V_previous)
        {
            /* code */
            Pno->Vref += Pno->dV;
        }
        else
        {
            /* code */
            Pno->Vref -= Pno->dV;
        }
    }
    if (Pno->Vref>=32.9)
    {
        /* code */
        Pno->Vref=32.9;
    }
    if (Pno->Vref<=0)
    {
        /* code */
        Pno->Vref=0;
    }
    Pno->P_previous = Pno->P_current;
    Pno->V_previous = Pno->V_current;
}

//Increment and conductance method
void mpptInc(mpptVariables *Inc ,float Vpv, float Ipv)
{
    //Sense the Vpv and the Ipv values
    Inc->V_current= Vpv;
    Inc->I_current= Ipv;

    //Calculate dI and dv values
    Inc->dV = Inc->V_current - Inc->V_previous;
    Inc->dI = Inc->I_current - Inc->I_previous;

    //Judge the dV
    if (Inc->dV == 0)
    {
        //if dV = 0;
        //Judge the dI
        if (Inc->dI == 0)
        {
            //dI = 0;
            //Reach the maximum power point
        }
        //dI != 0;
        else
        {
            //Judge the dI > 0;
            //if (dI > 0)
            if(Inc->dI > 0)
            {
                Inc->Vref = Inc->Vref + Inc->step;
            }
            //dI <= 0;
            else
            {
                Inc->Vref = Inc->Vref - Inc->step;
            }
        }
    }
    //dV != 0;
    else
    {
        //Jude the dI/dV values is equal -I/V;
        if(Inc->dI/Inc->dV == -Inc->I_current/Inc->V_current)
        {

        }
        else
        {
            //Jude the dI/dV > -I/V;
            if(Inc->dI/Inc->dV > -Inc->I_current/Inc->V_current)
            {
                Inc->Vref = Inc->Vref + Inc->step;
            }
            else
            {
                Inc->Vref = Inc->Vref - Inc->step;
            }
        }
    }
    //Limit the maximum and minimum
    if (Inc->Vref>=32.9)
    {
        /* code */
        Inc->Vref=32.9;
    }
    if (Inc->Vref<=0)
    {
        /* code */
        Inc->Vref=0;
    }
    //Transform the new value to the new old value
    Inc->V_previous = Inc->V_current;
    Inc->I_previous = Inc->I_current;
}
