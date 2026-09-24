#include "detector.h"
#include "receiver/globals.h"

#pragma region Constructor {

/**
 * @brief Detector::Detector
 */
Detector::Detector()
{
    //    dataExtractor=new DataExtractor();

    // Create an empty list of pulse object
    pulseList = new QList<Pulse*>();
    temppulseList = new QList<Pulse*>();
    //    holdingPls= new QList<Pulse*>();

    yItemsQueue=new QList<QVector<double>>();

    FreqToaMap=new QMap<double,double>();

    pow2X = new QVector<long int>(0);
    pow2X->append(0);
    for (int i = 1; i <= 50; i++)
        pow2X->append(pow(2, i));

    NumWordsX = Constants::numWordsFsX;

    ev = (fsX / acquisitiondecrateX);
    ev2 = (FIRFilterGainX * CICGainMismatchX * pow2X->at(13));

    pulseVector.resize(33);
}

#pragma endregion }

#pragma region Operations {

/**
 * @brief Detector::setTunerFrequency
 * @param newTunerFrequency
 */
void Detector::setTunerFrequency(double newTunerFrequency)
{
    ft = newTunerFrequency;
}

void Detector::setStop(bool newStop)
{
    stop = newStop;
}

void Detector::calculatingPRI(Pulse pls)
{
    int chNo = int(pls.Channel);
    size_t plsSize = pulseVector.at(chNo).size();

    if(plsSize > 0 and plsSize < 100)
    {
        int countChannel = pulseVector.at(chNo).back().Count;
        pls.diffToa =  pls.TOA - pulseVector.at(chNo).back().TOA;
        //        qDebug()<< "===>" <<QString::number(pls.diffToa,'g');
        //                std::cout<<"===>" << pls.diffToa <<std::endl;
        pulseVector.at(chNo).push_back(pls);
        pulseVector.at(chNo).back().Count = countChannel + 1;
    }

    else
    {
        if (plsSize >= 100)
        {
            int sze = plsSize;
            long double sumAvg{0};
            for (int i{1}; i < int(plsSize); i++)
            {
                if((pulseVector.at(chNo).at(i).diffToa < 20000) && (pulseVector.at(chNo).at(i).diffToa > 0))
                {
                    sumAvg += pulseVector.at(chNo).at(i).diffToa;
                }

                else {
                    sze --;
                }
            }

            sumAvg /= sze ;


            QList<Pulse *> *val;
            val = new QList<Pulse *> ;

            //            qInfo()<<"Diff1 main ===>"<<QString::number(pulseVector.at(chNo).back().diffToa,'g',16);

            //            qInfo()<<"sumAvg ===>"<<QString::number(sumAvg,'g');
            //            qInfo()<<"sumAvg ===>"<<QString::number(plsSize,'g');
            pulseVector.at(chNo).back().diffToa= sumAvg;
            val->push_back(&pulseVector.at(chNo).back());

            //            qInfo()<<"Diff2 main ===>"<<QString::number(pulseVector.at(chNo).back().diffToa,'g',16);
            //            qInfo()<<"Diff3 ===>"<<QString::number(val->at(0)->diffToa,'g',16);

            emit showList(val);

            pulseVector[chNo].clear();

            //            qInfo()<<"Diff6 main ===>"<<QString::number(pulseVector.at(chNo).back().diffToa,'g',16);
            //            qInfo()<<"Diff7 ===================================>"<<QString::number(val->at(0)->diffToa,'g',16);
        }

        else
        {
            pulseVector[chNo].push_back(pls);
            //            qInfo()<<"ch2"<<QString::number(pulseVector.at(chNo).back().TOA,'g',16);

        }

    }
}

/**
 * @brief Detector::CheckPulse
 * @param pulse
 * @param updateFreqMap
 */
bool Detector::CheckPulse(Pulse *pulse,bool updateFreqMap=false)
{
    //     qInfo()<<"----->4 " << pulse->Freq;

    bool isExist=false;

    for (int i = 0; i < pulseList->size(); i++) {

        if(abs (pulseList->at(i)->Freq - pulse->Freq) < 20 )
        {
            pulseList->at(i)->Update(pulse);

            // Update pulse freq map
            if(updateFreqMap)
            {
                FreqToaMap->remove(pulseList->at(i)->Freq);
                FreqToaMap->insert(pulse->Freq,pulseList->at(i)->TOA);
            }
            else
                pulseList->at(i)->Freq=pulse->Freq;
            isExist=true;
            //             qInfo()<<"----->51 " << pulse->Freq;
            break;
        }
    }

    if(!isExist)
    {
        if(!updateFreqMap)
            pulseList->append(pulse);

        FreqToaMap->insert(pulse->Freq,pulse->TOA);

        //         qInfo()<<"----->52 " << pulse->Freq;
        return true;
    }



    //================== [] ==================

    //========================================
    //     qInfo()<<"----->53 " << pulse->Freq;
    return false;
}

/**
 * @brief CountLessThan : comparator pulse count. use for sort pulse list by pulse count
 * @param p1 : first pulse
 * @param p2 : next pulse
 * @return true if p1's count is less than p2's count
 */
bool CountLessThan(const Pulse *p1, const Pulse *p2)
{
    return p1->Count < p2->Count;
}

/**
 * @brief Detector::ClearUnusedFreqs
 */
void Detector::ClearUnusedFreqs()
{
    mutex.lock();
    std::sort(pulseList->begin(),pulseList->end(),CountLessThan);
    temppulseList->clear();

    int minSize=yItemsQueue->at(0).size()/1000;
    int minSize2=yItemsQueue->at(0).size()/1000;

    for (int i=0;i<pulseList->size();i++)
    {
        if(QString::number(pulseList->at(i)->PAPeak)=="nan")
        {
            if(pulseList->at(i)->Count>25) //raziani
                //             if(pulseList->at(i)->Count>1)
                temppulseList->append(pulseList->at(i));
            else
                FreqToaMap->remove(pulseList->at(i)->Freq);
        }
        else
        {
            if(pulseList->at(i)->Count>5) //raziani
                //              if(pulseList->at(i)->Count>1)
                temppulseList->append(pulseList->at(i));
            else
                FreqToaMap->remove(pulseList->at(i)->Freq);
        }
    }

    pulseList->clear();
    for (int i=0;i<temppulseList->size();i++)
        pulseList->append(temppulseList->at(i));

    mutex.unlock();

}

/**
 * @brief Detector::AddToBuffer
 * @param yItems
 */
void Detector::AddToQueue(QVector<double> yItems)
{
    mutex.lock();
//    qInfo() <<"------------------------------------------------\t"<< yItems;
    QVector<double> items=yItems;
    yItemsQueue->append(items);
    mutex.unlock();
}

/**
 * @brief Detector::ReadBlockDataX
 * @param yItems
 * @param tunerFrequency
 */

void Detector::ReadBlockDataX(QVector<double> yItems)
{
    // Hold pattern Items
    QList<QVector<double>*> *patternListItems = new QList<QVector<double>*>();

    // create an empty pulse object
    Pulse *pulse;

    // Extract pattern Items from raw data
    for(int i=0;i<yItems.size()-1;i++)
    {
        if((yItems[i]!=pulseIndicator) &&
                (yItems[i]!=cwIndicator))
        {
            QVector<double>* Items=new QVector<double>();

            for(int j=i;j<i+16;j++)
            {

                if(j<yItems.size())
                    Items->append(yItems[j]);
            }

            if(Items->size()==16 && i>1)
            {
                // pulse => 1 , cw => 0
                Items->append(yItems[i-1]==pulseIndicator?1:0);
                patternListItems->append(Items);
            }

            i+=30;
        }
    }

    for(int i=0;i<patternListItems->size();i++)
    {
        QVector<double> *item= patternListItems->at(i);

        if(item->at(0)!=0)
        {
            pulse=new Pulse(Enumerations::Band::X);

            pulse->Channel=item->at(1);

            if(pulse->Channel>32 || pulse->Channel<0)
            {
                delete pulse;
                continue;

            }
            //sajad bahmani=========
            fr=(item->at(0)-alpha)*fs/1024;

            if(fr>(fs/2))
                fr-=fs;

            //new dorafshan
            if (pulse->Channel>=9  && pulse->Channel<=15 && fr+ft <ft ){
                pulse->Freq=fr+ft+246;
                //dorafshan
              //  qInfo()<<"fr negative = "<<fr << "\n\r";
            }
            else if( pulse->Channel<24 &&  pulse->Channel>15 && fr+ft >ft  )
                //pulse->Freq=fr+ft-246;
                pulse->Freq=fr+ft-246;

            // else if( pulse->Channel>=24 && fr+ft <ft  ){
            //dorafshan
            else if(   fr+ft <ft  ){
               // qInfo()<<"fr negative = "<<fr << "\n\r";
                pulse->Freq=fr+ft -0;
            }
            //==================

            else if( pulse->Channel<=15 &&  fr+ft >ft && pulse->Channel>=0  ){
               // qInfo()<<"fr negative = "<<fr << "\n\r";
                pulse->Freq=fr+ft -0;
            }

            else {
                pulse->Freq=fr+ft;
            }

            if(pulse->Freq<0)
            {
                delete pulse;
                continue;
            }
//            qInfo()<<"frq" <<QString::number(pulse->Freq,'f',16);

            // Update Freq map by checking frequency
            CheckPulse(pulse,true);
            //===========================================
            unsigned int v1 = static_cast<unsigned int>(item->at(4))*pow2X->at(32)/fs;
            unsigned int v2 = static_cast<unsigned int>(item->at(3))*pow2X->at(16)/fs;
            unsigned int v3 = static_cast<unsigned int>(item->at(2))/fs;

            pulse->TOA = static_cast<long double>(v1+v2+v3) ;

            //            qInfo()<<"-----> " << QString::number(pulse->TOA,'g',10);

            //===================================================

            PASum =((unsigned short)item->at(7))*pow2X->at(32);
            PASum+=((unsigned short)item->at(6))*pow2X->at(16);
            PASum+=(unsigned short)item->at(5);
            //            double PAPeak =((unsigned short)item->at(15))*pow2X->at(16);
            //            PAPeak+=(unsigned short)item->at(14);
            PAPeak =(unsigned short)item->at(14);
            //            bool CW = item->at(15)== 0;

//            unsigned short pp= (unsigned short)(item->at(15));
            bool CW = (((unsigned short)(item->at(15))) & 0x01)!=0;

            if (CW)
            {
                PRI_cal = 0;
                PW1 = 0;
                PW2 = 0;
                SelPW = PW2;

                PW2 = 1;

                pulse->PRI = PRI_cal;
                pulse->PW1 = SelPW / ev * 1E6;
                pulse->PAPeak = 20 * log10(PASum);
                pulse->Mode = (int)Enumerations::SignalType::CW;
                pulse->Freq2=globals::peakValue;
                pulse->Db2=globals::peakValueDb;
            }


            else
            {
                PRI_cal0 = pulse->TOA - FreqToaMap->value(pulse->Freq);
                PRI_cal = ((PRI_cal0 * 1e6) / ev);
                FreqToaMap->value(pulse->Freq,pulse->TOA);

                PW1 =((unsigned short)item->at(10))*pow2X->at(32);
                PW1+=((unsigned short)item->at(9))*pow2X->at(16);
                PW1+=(unsigned short)item->at(8);

                PW2 =((unsigned short)item->at(13))*pow2X->at(32);
                PW2+=((unsigned short)item->at(12))*pow2X->at(16);
                PW2+=(unsigned short)item->at(11);

                PW1 = PW1 / fs-0.1;
                PW2 = PW2 / fs;

                if(PW1<=0 || PW2<=0)
                {
                    delete pulse;
                    continue;
                }

                //                if (1 <= PRI_cal && PRI_cal < 20000 && 1 < SelPW && SelPW < 5000000)
                //                {
                pulse->PRI = PRI_cal;
                pulse->PW1 = PW1;
                pulse->PW2 = PW2;
                pulse->PAPeak = PAPeak;
                pulse->PASum = PASum;
                pulse->Mode = (int)Enumerations::SignalType::Pulse;
                pulse->Freq2=globals::peakValue;
                pulse->Db2=globals::peakValueDb;
//                qInfo()<<"----->1 " << pulse->Mode;
            }
                calculatingPRI(*pulse);
        }
    }

    delete patternListItems;
    //    qDebug()<<"==========================================================>" ;
    //        emit newList(pulseList);
    mutex.unlock();
}

//void Detector::ReadBlockDataX(QVector<double> yItems)
//{


//    //        qInfo() <<"/*/*/*/----> "<< yItems;
//    //    qInfo() << "------------------------------------------------------------------------------------------------------";



//    // Hold pattern Items
//    QList<QVector<double>*> *patternListItems=new QList<QVector<double>*>();

//    // create an empty pulse object
//    Pulse *pulse;

//    // Extract pattern Items from raw data
//    for(int i=0;i<yItems.size()-1;i++)
//    {
//        if((yItems[i]!=pulseIndicator) && (yItems[i]!=cwIndicator))
//        {
//            QVector<double>* Items=new QVector<double>();

//            for(int j=i;j<i+16;j++)
//            {

//                if(j<yItems.size()) Items->append(yItems[j]);
//            }

//            if(Items->size()==16 && i>1)
//            {
//                // pulse => 1 , cw => 0
//                Items->append(yItems[i-1]==pulseIndicator?1:0);

//                patternListItems->append(Items);
//            }

//            i+=15;
//        }
//    }


//    //    qInfo() <<"/*/*/*/----> "<< Items;

//    for(int i=0;i<patternListItems->size();i++)
//    {
//        QVector<double> *item= patternListItems->at(i);

//        if(item->at(0)!=0)
//        {
//            pulse=new Pulse(Enumerations::Band::X);

//            pulse->Channel=item->at(1);

//            if(pulse->Channel>32 || pulse->Channel<0)
//            {
//                delete pulse;
//                continue;
//            }


//            //            fr=(item->at(0)-alpha)*fs/1024;

//            //            if(fr>(fs/2))
//            //                fr-=fs;
//            //            if (pulse->Channel>=9  && pulse->Channel<=15 && fr+ft <ft ){
//            //                pulse->Freq=fr+ft+246;}
//            //            else if( pulse->Channel<=24 &&  pulse->Channel>15 && fr+ft >ft  )
//            //                pulse->Freq=fr+ft-246;

//            //sajad bahmani=========
//            fr=(item->at(0)-alpha)*fs/1024;

//            if(fr>(fs/2)) fr-=fs;

//            if(pulse->Freq<0)
//            {
//                delete pulse;
//                continue;
//            }

//            //            if (pulse->Channel>=9  && pulse->Channel<=15 && fr+ft <ft ){
//            //                pulse->Freq=fr+ft+246;
//            //                //dorafshan
//            //                qInfo()<<"fr negative = "<<fr << "\n\r";
//            //            }
//            //            else if( pulse->Channel<24 &&  pulse->Channel>15 && fr+ft >ft  )
//            //                //pulse->Freq=fr+ft-246;
//            //                pulse->Freq=fr+ft-246;

//            //            // else if( pulse->Channel>=24 && fr+ft <ft  ){
//            //            //dorafshan
//            //            else if(   fr+ft <ft  ){
//            //                qInfo()<<"fr negative = "<<fr << "\n\r";
//            //                pulse->Freq=fr+ft -0;
//            //            }
//            //            //==================

//            //            else if( pulse->Channel<=11 &&  fr+ft >ft  ){
//            //                qInfo()<<"fr negative = "<<fr << "\n\r";
//            //                pulse->Freq=fr+ft -246;
//            //            }

//            //            else {
//            //                pulse->Freq=fr+ft;
//            //            }

//            //            if(pulse->Freq<0)
//            //            {
//            //                delete pulse;
//            //                continue;
//            //            }
//            if (pulse->Channel>=9  && pulse->Channel<=15 && fr+ft <ft )
//            {
//                pulse->Freq=fr+ft+246;
//                //dorafshan
//                //  qInfo()<<"fr negative = "<<fr << "\n\r";
//            }
//            else if( pulse->Channel<24 &&  pulse->Channel>15 && fr+ft >ft)
//                //pulse->Freq=fr+ft-246;
//                pulse->Freq=fr+ft-246;

//            // else if( pulse->Channel>=24 && fr+ft <ft  ){
//            //dorafshan
//            else if(   fr+ft <ft)
//            {
//                // qInfo()<<"fr negative = "<<fr << "\n\r";
//                pulse->Freq=fr+ft -0;
//            }
//            //==================

//            else if( pulse->Channel<=15 &&  fr+ft >ft && pulse->Channel>=0)
//            {
//                // qInfo()<<"fr negative = "<<fr << "\n\r";
//                pulse->Freq=fr+ft -0;
//            }

//            else {
//                pulse->Freq=fr+ft;
//            }

//            if(pulse->Freq<0)
//            {
//                delete pulse;
//                continue;
//            }
//            //
//            //            qInfo()<<"frq" <<QString::number(pulse->Freq,'f',16);

//            // Update Freq map by checking frequency
//            CheckPulse(pulse,true);

//            //=============saeid raziani======================================
//            //            pulse->TOA=item->at(4)*pow2X->at(32)/fs;
//            //            pulse->TOA+=item->at(3)*pow2X->at(16)/fs;
//            //            pulse->TOA+=item->at(2)/fs;

//            //===========================================
//            unsigned int v1 = static_cast<unsigned int>(item->at(4))*pow2X->at(32)/fs;
//            unsigned int v2 = static_cast<unsigned int>(item->at(3))*pow2X->at(16)/fs;
//            unsigned int v3 = static_cast<unsigned int>(item->at(2))/fs;
//            pulse->TOA =static_cast<long double>(v1+v2+v3) ;



//            std::cout<< "~~~~~>" << (    ( (item->at(4))*pow2X->at(32)/fs) +( (item->at(3))*pow2X->at(16)/fs) +    (item->at(2)/fs) ) - test <<std::endl;

//                        test = ((item->at(4))*pow2X->at(32)/fs) +( (item->at(3))*pow2X->at(16)/fs) +    ((item->at(2))/fs) ;
//            //===================================================

//            PASum =((unsigned short)item->at(7))*pow2X->at(32);
//            PASum+=((unsigned short)item->at(6))*pow2X->at(16);
//            PASum+=(unsigned short)item->at(5);
//            //            double PAPeak =((unsigned short)item->at(15))*pow2X->at(16);
//            //            PAPeak+=(unsigned short)item->at(14);
//            PAPeak =(unsigned short)item->at(14);
//            //            bool CW = item->at(15)== 0;

//            //            unsigned short pp= (unsigned short)(item->at(15));
//            bool CW = (((unsigned short)(item->at(15))) & 0x01)!=0;

//            if (CW)
//            {
//                PRI_cal = 0;
//                PW1 = 0;
//                PW2 = 0;
//                SelPW = PW2;

//                PW2 = 1;

//                pulse->PRI = PRI_cal;
//                pulse->PW1 = SelPW / ev * 1E6;
//                pulse->PAPeak = 20 * log10(PASum);
//                pulse->Mode = (int)Enumerations::SignalType::CW;
//                pulse->Freq2=globals::peakValue;
//                pulse->Db2=globals::peakValueDb;
//            }


//            else
//            {
//                PRI_cal0 = pulse->TOA - FreqToaMap->value(pulse->Freq);
//                PRI_cal = ((PRI_cal0 * 1e6) / ev);
//                FreqToaMap->value(pulse->Freq,pulse->TOA);

//                PW1 =((unsigned short)item->at(10))*pow2X->at(32);
//                PW1+=((unsigned short)item->at(9))*pow2X->at(16);
//                PW1+=(unsigned short)item->at(8);

//                PW2 =((unsigned short)item->at(13))*pow2X->at(32);
//                PW2+=((unsigned short)item->at(12))*pow2X->at(16);
//                PW2+=(unsigned short)item->at(11);

//                PW1 = PW1 / fs-0.1;
//                PW2 = PW2 / fs;

//                if(PW1<=0 || PW2<=0)
//                {
//                    delete pulse;
//                    continue;
//                }

//                //                if (1 <= PRI_cal && PRI_cal < 20000 && 1 < SelPW && SelPW < 5000000)
//                //                {
//                pulse->PRI = PRI_cal;
//                pulse->PW1 = PW1;
//                pulse->PW2 = PW2;
//                pulse->PAPeak = PAPeak;
//                pulse->PASum = PASum;
//                pulse->Mode = (int)Enumerations::SignalType::Pulse;
//                pulse->Freq2=globals::peakValue;
//                pulse->Db2=globals::peakValueDb;
//                //                qInfo()<<"----->1 " << pulse->Freq;

//            }


//            calculatingPRI(*pulse);

//            if(!CheckPulse(pulse))
//            {
//                delete pulse;
//            }
//            else
//            {


//            }


//        }
//    }

//    delete patternListItems;
//    //    qDebug()<<"==========================================================>" ;
//    //        emit newList(pulseList);
//    mutex.unlock();
//}

/**
 * @brief Thread main function
 */
void Detector::run()
{
    pulseList->clear();
    FreqToaMap->clear();
    stop=false;

    emit newList(pulseList);


    while(!stop)
    {
        if(!Processing)


            if(yItemsQueue->size()>0)
            {
                Processing=true;

                if(clearCounter==30)
                {
                    ClearUnusedFreqs();
                    clearCounter=0;
                }

                else
                {
                    clearCounter++;
                    mutex.lock();
                    ReadBlockDataX(yItemsQueue->at(0));
                    yItemsQueue->removeFirst();
                }

                Processing=false;
            }

        usleep(10);
    }

}

#pragma endregion }

#pragma region Utility {

std::string double2hexstr(double x) {

    union
    {
        long long i;
        double    d;
    } value;

    value.d = x;

    char buf[17];

    snprintf (buf,sizeof(buf),"%016llx",value.i);
    buf[16]=0; //make sure it is null terminated.

    return std::string(buf);

}

double hexstr2double(const std::string& hexstr)
{
    union
    {
        long long i;
        double    d;
    } value;

    value.i = std::stoll(hexstr, nullptr, 16);

    return value.d;
}

#pragma endregion }






//if(pulseList->size() > 0)
//{
////        qInfo()<<"new "<<pulse->Channel;

//    for(int i{};i<holdingPls.size();i++)
//    {
//        if (holdingPls.at(i)->Channel == pulse->Channel)
//        {
//            duplicate = true;
//            qInfo()<<"update list ";
//            holdingPls[i] = pulse;
//            foreach (auto var, holdingPls)
//            {
//                qInfo()<<"=>"<<var->Channel;
//            }
//        }
//        else {
//            duplicate = false;
//        }

//    }


//    if(!duplicate)
//    {
//        qInfo()<<"adding new one to list" ;
//        holdingPls.append(pulse);
//    }
//    qInfo()<<"size: "<<holdingPls.size();
//}
//else
//{
//    holdingPls.append(pulse);
//}
