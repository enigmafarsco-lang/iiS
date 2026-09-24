#include "pulse.h"

#pragma region Constructors {

/**
 * @brief Pulse::Pulse
 */
Pulse::Pulse()
{

}

/**
 * @brief Pulse::Pulse
 * @param pulseBand
 */
Pulse::Pulse(Enumerations::Band pulseBand)
{
    switch (pulseBand)
    {
    case Enumerations::Band::L:
        MinFrequency = Constants::MinFrequency_L;
        MaxFrequency = Constants::MaxFrequency_L;
        break;
    case Enumerations::Band::S:
        MinFrequency = Constants::MinFrequency_S;
        MaxFrequency = Constants::MaxFrequency_S;
        break;
    case Enumerations::Band::X:
        MinFrequency = Constants::MinFrequency_X;
        MaxFrequency = Constants::MaxFrequency_X;
        break;
    case Enumerations::Band::None:
    default:
        MinFrequency = 0;
        MaxFrequency = 0;
        break;
    }
}

#pragma endregion }

#pragma region Operations {

/**
 * @brief Pulse::Clone
 * @param inputPulse
 * @return
 */
QList<Pulse*> *Pulse::Clone(QList<Pulse*> *inputPulse)
{
    if (inputPulse != nullptr)
    {
        Pulse *onePulse;
        QList<Pulse*> *exportPulse = new QList<Pulse*>();
        for (int i = 0; i < inputPulse->count(); i++)
        {
            onePulse = new Pulse();
            onePulse->CNP       = inputPulse->at(i)->CNP;
            onePulse->Freq      = inputPulse->at(i)->Freq;
            onePulse->Freq2      = inputPulse->at(i)->Freq2;
            onePulse->Db2      = inputPulse->at(i)->Db2;
            onePulse->PAPeak    = inputPulse->at(i)->PAPeak;
            onePulse->PW1       = inputPulse->at(i)->PW1;
            onePulse->PW2       = inputPulse->at(i)->PW2;
            onePulse->PRI       = inputPulse->at(i)->PRI;
            onePulse->PRF       = inputPulse->at(i)->PRF;
            onePulse->Mode      = inputPulse->at(i)->Mode;

            exportPulse->append(onePulse);
        }
        return exportPulse;
    }
    return nullptr;
}

void Pulse::Update(Pulse *p)
{
    this->Channel=p->Channel;
    this->Freq2 =p->Freq2;
    this->Db2 =p->Db2;
    this->TOA =p->TOA;
    this->CNP=p->CNP;
    this->PAPeak=p->PAPeak;
    this->PW1=p->PW1;
    this->PW2=p->PW2;
    this->PRI=p->PRI;
    this->PRF=p->PRF;
    this->Mode=p->Mode;
    this->Count++;
}

#pragma endregion }
