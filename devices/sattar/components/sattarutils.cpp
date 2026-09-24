#include "sattarutils.h"



bool SattarUtils::isBtnColorGreen[4]{false};
QString SattarUtils::filePath{};
int SattarUtils::N{7000};
int SattarUtils::normalSample{} ;

SattarUtils::SattarUtils(QObject *parent) : QObject(parent)
{

}


//------------------ Utils functions --------------------------
float SattarUtils::frqToTime(QString priBase)
{
    QStringList priSpl = priBase.split(" ");
    double priVal = priSpl[0].toDouble();

    priVal = 1 / priVal; // convert to time

    if (priSpl[1] == "MHz")
    {
        return  priVal * 1000;
    }

    else if (priSpl[1] == "Hz") {
        return  priVal * 1000000000;

    }

    else if (priSpl[1] == "KHz") {

        return  priVal * 1000000;
    }
    return 0;
}

float SattarUtils::timeToFloat(QString priBase)
{

    QStringList priSpl = priBase.split(" ");
    double priVal = priSpl[0].toDouble();

    if (priSpl[1] == "uS")
    {
        return  priVal * 1000;
    }


    else if (priSpl[1] == "mS") {
        return  priVal * 1000000;
    }

    else if (priSpl[1] == "nS") {
        return  priVal * 1;
    }
    return 0;
}

ulong SattarUtils::frqToULong(QString priBase)
{
    QStringList priSpl = priBase.split(" ");
    double priVal = priSpl[0].toDouble();

    if (priSpl[1] == "MHz")
    {
        return  static_cast<ulong>(priVal * 1000000);
    }

    else if (priSpl[1] == "Hz")
    {
        return  static_cast<ulong>(priVal *1);
    }

    else if (priSpl[1] == "KHz")
    {
        return  static_cast<ulong>(priVal * 1000);
    }

    return 0;
}

ulong SattarUtils::timeToULong(const uint &time, const QString &measUnit)
{

    if (measUnit == "nS")
    {
        return time;
    }

    else if (measUnit == "uS") {

        return time * 1000;
    }

    else if (measUnit == "mS") {

        return time * 1000000;
    }

    else if (measUnit == "S") {

        return time * 1000000000;
    }

    return time;
}

uint SattarUtils::timeToInt(const uint &time, const QString &measUnit)
{
    if (measUnit == "nS")
    {
        return time;
    }

    else if (measUnit == "uS") {

        return time * 1000;
    }

    else if (measUnit == "mS") {

        return time * 1000000;
    }

    else if (measUnit == "S") {

        return time * 1000000000;
    }

    return time;
}

void SattarUtils::msgFunction(QString msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

}

QString SattarUtils::floatToTime(float &val)
{
    QString tmp{};
    if(val >= 1000)
    {
        val /= 1000;
        tmp = "uS";

        if(val >=1000)
        {
            val /= 1000;
            tmp = "mS";

            if(val >=1000)
            {
                val /= 1000;
                tmp = "S";
            }
        }
    }
    else
    {
        tmp = "nS";
    }

    return (QString::number(val) +" "+ tmp);
}

//generating data to send to dll
void SattarUtils::generateRadarData(radarSpace::radarDataStruct &radarData,
                                    modulSpace::modulDataStruct &modulData,
                                    spureSpace::spureStruct &spurData,
                                    priSpace::priDataStruct &priData,
                                    frqSpace::frqDataStruct &frqData,
                                    pwSpace::pwDataStruct& pwData,
                                    projectDataSpace::projectDataStruct& prjData ,
                                    int status
                                    )
{


    this->status = status;


    //----------------MissPulse---------------------------
    QJsonObject missJson;

    //----------------FREQ---------------------------
    QJsonArray divArr;
    QJsonArray frqArr;
    QJsonObject frqJson;

    for (int i{}; i < frqData.frq.count(); i++)
    {
        frqArr.append(frqData.frq.at(i));
    }

    for (int i{}; i < frqData.diversity.count(); i++)
    {
        QJsonObject tmp;
        tmp["multiplier"] = static_cast<int>(frqData.diversity[i].multiplier);
        tmp["freq"] = frqData.diversity[i].frq;
        divArr.append(tmp);
    }


    frqJson["type"]         = frqData.frqType;
    frqJson["freq"]         = frqArr;
    frqJson["diversity"]    = divArr;
    frqJson["baseFreq"]     = frqData.baseFrq;
    frqJson["countFreq"]    = static_cast<int>(frqData.countFrq);
    frqJson["maxFreq"]      = frqData.maxFrq;
    frqJson["minFreq"]      = frqData.minFrq;
    frqJson["agilePercent"] = frqData.agilePercent;
    frqJson["agileCount"]   = static_cast<int>(frqData.agileCount);


    //----------------Modulation---------------------
    QJsonObject modulJson;

    modulJson["LFMType"]          = modulData.LFMSweetDirection;
    modulJson["bpskTB"]           = modulData.bpskTB;
    modulJson["bpskType"]         = modulData.bpsKType;
    modulJson["bpskbarkerCode"]   = static_cast<int>(modulData.bpskBarkerCode);
    modulJson["bpskcustomCode"]   = modulData.bpskCustomCode;
    modulJson["fskSDeltaF"]       = modulData.fskDeltaF;
    modulJson["fskSimpleBitInfo"] = modulData.fskSimpleBitInfo;
    modulJson["fskSimpleTB"]      = modulData.fskSimpleTB;
    modulJson["fskSlope"]         = static_cast<int>(modulData.fskSlope);
    modulJson["fskStepCostas"]    = modulData.fskStepCostas;
    modulJson["fskType"]          = modulData.FSKType;
    modulJson["isReversed"]       = modulData.isReversed;
    modulJson["qpskCustomCode"]   = modulData.qpskCustomCode;
    modulJson["qpskTB"]           = modulData.qpskTB;
    modulJson["qpskType"]         = modulData.qpskType;
    modulJson["qpskfrankCode"]    = static_cast<int>(modulData.qpskFrankCode);
    modulJson["type"]             = modulData.type;
    modulJson["xfmTargetFreq"]    = modulData.xfmTargetFrq;
    modulJson["xfmTargetTime"]    = modulData.xfmTargetTime;
    modulJson["xfmType"]          = modulData.xFMType;




    //------------------PRI--------------------------
    QJsonArray PRIArr;
    QJsonArray dwellArr;

    for (int i{}; i < priData.pri.count(); i++)
    {
        PRIArr.append(priData.pri.at(i));
    }

    for (int i{}; i < priData.dwell.count(); i++)
    {
        QJsonObject tmp;
        tmp["multiplier"] = static_cast<int>(priData.dwell[i].multiplier);
        tmp["pri"] = priData.dwell[i].pri;
        tmp["uint"] = priData.dwell[i].unit;
        dwellArr.append(tmp);
    }


    QJsonObject PRIJson;
//    QStringList l =priData.priBase.split(" ");
//    normalSample = l[0].toDouble() * 449;


    priData.priBase = QString::number(priData.priBase.toDouble() * 225);
    PRIJson["dwell"]           = dwellArr;
    PRIJson["pri"]             = PRIArr;
    PRIJson["maxPri"]          = priData.maxPri;
    PRIJson["minPri"]          = priData.minPri;
    PRIJson["countPri"]        = static_cast<int>(priData.CountPri);
    PRIJson["priBase"]         = priData.priBase;
    PRIJson["jitterPercent"]   = static_cast<int>(priData.jitterPercent);
    PRIJson["jitterCount"]     = static_cast<int>(priData.jitterCount);
    PRIJson["slidingCount"]    = static_cast<int>(priData.slidingCount);
    PRIJson["slidingStep"]     = priData.slidingStep;
    PRIJson["triangularCount"] = static_cast<int>(priData.triangularCount);
    PRIJson["triangularStep"]  = priData.triangularStep;
    PRIJson["periodicMean"]    = static_cast<int>(priData.periodicMean);
    PRIJson["periodicFreq"]    = priData.periodicFreq;
    PRIJson["periodicCount"]   = static_cast<int>(priData.periodicCount);
    PRIJson["type"]            = priData.type;
    PRIJson["priftype"]        = priData.prifType;





    //------------------PW---------------------------
    QJsonArray PWArr;
    QJsonArray multiArr;

    for (int i{}; i < pwData.pw.count(); i++)
    {
        PWArr.append(static_cast<int>(pwData.pw.at(i)));
    }

    for (int i{}; i < pwData.multiLevel.count(); i++)
    {
        QJsonObject tmp;
        tmp["multiplier"] = static_cast<int>(pwData.multiLevel[i].multiplier);
        tmp["pw"] = static_cast<int>(pwData.multiLevel[i].pw);
        tmp["uint"] = pwData.multiLevel[i].unint;
        multiArr.append(tmp);
    }

    QJsonObject PWJson;

    PWJson["type"]            = pwData.Type;
    PWJson["pw"]              = PWArr;
    PWJson["multilevel"]      = multiArr;
    PWJson["pwConstant"]      = static_cast<int>(pwData.pwConstant);
    PWJson["maxPw"]           = static_cast<int>(pwData.maxPw);
    PWJson["minPw"]           = static_cast<int>(pwData.minPw);
    PWJson["countPw"]         = static_cast<int>(pwData.countPw);
    PWJson["randomPwPercent"] = static_cast<int>(pwData.randomPwPercent);
    PWJson["randomPwCount"]   = static_cast<int>(pwData.randomPwCount);





    //---------------------Spur--------------------------
    QJsonObject spurJson;


    {
        //********pw****************
        QJsonArray PWArr;
        QJsonArray multiArr;

        for (int i{}; i < spurData.pwSpure.pw.count(); i++)
        {
            PWArr.append(static_cast<int>(spurData.pwSpure.pw.at(i)));
        }

        for (int i{}; i < spurData.pwSpure.multiLevel.count(); i++)
        {
            QJsonObject tmp;
            tmp["multiplier"] = static_cast<int>(spurData.pwSpure.multiLevel[i].multiplier);
            tmp["pw"] = static_cast<int>(spurData.pwSpure.multiLevel[i].pw);
            tmp["uint"] = spurData.pwSpure.multiLevel[i].unint;
            multiArr.append(tmp);
        }

        QJsonObject PWJson;
        PWJson["type"]            = spurData.pwSpure.Type;
        PWJson["pw"]              = PWArr;
        PWJson["multilevel"]      = multiArr;
        PWJson["pwConstant"]      = static_cast<int>(spurData.pwSpure.pwConstant);
        PWJson["maxPw"]           = static_cast<int>(spurData.pwSpure.maxPw);
        PWJson["minPw"]           = static_cast<int>(spurData.pwSpure.minPw);
        PWJson["countPw"]         = static_cast<int>(pwData.countPw);
        PWJson["randomPwPercent"] = static_cast<int>(spurData.pwSpure.randomPwPercent);
        PWJson["randomPwCount"]   = static_cast<int>(spurData.pwSpure.randomPwCount);

        spurJson["pw"]           = PWJson ;
    }









    {
        //********pri***************

        QJsonArray PRIArr;
        QJsonArray dwellArr;

        for (int i{}; i < spurData.priSpure.pri.count(); i++)
        {
            PRIArr.append(spurData.priSpure.pri.at(i));
        }

        for (int i{}; i < spurData.priSpure.dwell.count(); i++)
        {
            QJsonObject tmp;
            tmp["multiplier"] = static_cast<int>(spurData.priSpure.dwell[i].multiplier);
            tmp["pri"] = spurData.priSpure.dwell[i].pri;
            tmp["uint"] = spurData.priSpure.dwell[i].unit;
            dwellArr.append(tmp);
        }

        QJsonObject PRIJson;
        PRIJson["dwell"]           = dwellArr;
        PRIJson["pri"]             = PRIArr;
        PRIJson["maxPri"]          = spurData.priSpure.maxPri;
        PRIJson["minPri"]          = spurData.priSpure.minPri;
        PRIJson["countPri"]        = static_cast<int>(spurData.priSpure.CountPri);
        PRIJson["priBase"]         = spurData.priSpure.priBase;
        PRIJson["jitterPercent"]   = static_cast<int>(spurData.priSpure.jitterPercent);
        PRIJson["jitterCount"]     = static_cast<int>(spurData.priSpure.jitterCount);
        PRIJson["slidingCount"]    = static_cast<int>(spurData.priSpure.slidingCount);
        PRIJson["slidingStep"]     = spurData.priSpure.slidingStep;
        PRIJson["triangularCount"] = static_cast<int>(spurData.priSpure.triangularCount);
        PRIJson["triangularStep"]  = spurData.priSpure.triangularStep;
        PRIJson["periodicMean"]    = static_cast<int>(spurData.priSpure.periodicMean);
        PRIJson["periodicFreq"]    = static_cast<int>(spurData.priSpure.periodicFreq);
        PRIJson["periodicCount"]   = static_cast<int>(spurData.priSpure.periodicCount);
        PRIJson["type"]            = spurData.priSpure.type;
        PRIJson["priftype"]        = spurData.priSpure.prifType;
        spurJson["pri"]            = PRIJson ;


    }







    {
        //********frq******************
        QJsonArray divArr;
        QJsonArray frqArr;

        for (int i{}; i < spurData.frqSpure.frq.count(); i++)
        {
            frqArr.append(spurData.frqSpure.frq.at(i));
        }

        for (int i{}; i < spurData.frqSpure.diversity.count(); i++)
        {
            QJsonObject tmp;
            tmp["multiplier"] = static_cast<int>(spurData.frqSpure.diversity[i].multiplier);
            tmp["freq"] = spurData.frqSpure.diversity[i].frq;
            divArr.append(tmp);
        }

        QJsonObject frqJsonSpure;
        frqJsonSpure["type"]         = spurData.frqSpure.frqType;
        frqJsonSpure["freq"]         = frqArr;
        frqJsonSpure["diversity"]    = divArr;
        frqJsonSpure["baseFreq"]     = spurData.frqSpure.baseFrq;
        frqJsonSpure["countFreq"]    = static_cast<int>(spurData.frqSpure.countFrq);
        frqJsonSpure["maxFreq"]      = spurData.frqSpure.maxFrq;
        frqJsonSpure["minFreq"]      = spurData.frqSpure.minFrq;
        frqJsonSpure["agilePercent"] = spurData.frqSpure.agilePercent;
        frqJsonSpure["agileCount"]   = static_cast<int>(spurData.frqSpure.agileCount);
        spurJson["freq"]             = frqJsonSpure;
    }







    {
        //********modulation********
        QJsonObject modulJson;

        modulJson["type"]             = spurData.modulSpure.type;
        modulJson["xfmTargetFreq"]    = spurData.modulSpure.xfmTargetFrq;
        modulJson["xfmType"]          = spurData.modulSpure.xFMType;
        modulJson["LFMType"]          = spurData.modulSpure.LFMSweetDirection;
        modulJson["xfmTargetTime"]    = spurData.modulSpure.xfmTargetTime;
        modulJson["fskType"]          = spurData.modulSpure.FSKType;
        modulJson["fskSDeltaF"]       = spurData.modulSpure.fskDeltaF;
        modulJson["fskSimpleTB"]      = spurData.modulSpure.fskSimpleTB;
        modulJson["fskSimpleBitInfo"] = spurData.modulSpure.fskSimpleBitInfo;
        modulJson["fskStepCostas"]    = spurData.modulSpure.fskStepCostas;
        modulJson["fskSlope"]         = static_cast<int>(spurData.modulSpure.fskSlope);
        modulJson["bpskTB"]           = spurData.modulSpure.bpskTB;
        modulJson["bpskType"]         = spurData.modulSpure.bpsKType;
        modulJson["bpskbarkerCode"]   = static_cast<int>(spurData.modulSpure.bpskBarkerCode);
        modulJson["bpskcustomCode"]   = spurData.modulSpure.bpskCustomCode;
        modulJson["qpskTB"]           = spurData.modulSpure.qpskTB;
        modulJson["qpskType"]         = spurData.modulSpure.qpskType;
        modulJson["qpskfrankCode"]    = static_cast<int>(spurData.modulSpure.qpskFrankCode);
        modulJson["qpskCustomCode"]   = spurData.modulSpure.qpskCustomCode;
        modulJson["isReversed"]       = spurData.modulSpure.isReversed;
        spurJson["modulation"]        = modulJson ;
    }


    spurJson["spurPercent"] = static_cast<int>(spurData.spurePercent);
    spurJson["spurCount"]    = static_cast<int>(spurData.spureCount);




    //---------------------Radar Data--------------------------
    QJsonObject dataJson;
    dataJson["radarBand"]       = prjData.radarBand;
    dataJson["radarFreq"]       = prjData.radarFreq;
    dataJson["mode"]            = prjData.radarMode;
    dataJson["phaseI"]          = radarData.phaseI;
    dataJson["phaseQ"]          = radarData.phaseQ;
    dataJson["radarNumber"]     = static_cast<int>(radarData.radarNumber) ;
    dataJson["shiftTime"]       = static_cast<float>(radarData.shiftTime);
    dataJson["durationTime"]    = static_cast<float>(radarData.durationTime) ;
    dataJson["missPulse"]       = static_cast<int>(radarData.missPulse);
    dataJson["radarPriority"]   = static_cast<int>(radarData.radarPriority);
    dataJson["outputPower"]     = radarData.outPutPower;
    dataJson["maxPulseCount"]   = static_cast<int>(radarData.maxPulseCount);
    dataJson["generateAsync"]   = radarData.generateAsync;
    dataJson["generateSimData"] = radarData.generateSimData;
    dataJson["modulation"]      = modulJson;
    dataJson["pw"]              = PWJson;
    dataJson["pri"]             = PRIJson;
    dataJson["freq"]            = frqJson;
    dataJson["missPulseData"]   = missJson;
    dataJson["spurData"]        = spurJson;
    dataJson["radarName"]       = radarData.radarName;
    radarName                   = radarData.radarName;






    //----------------Final Data-------------------------------

    QJsonArray timingArr;
    QJsonArray dataArr;
    dataArr.append(dataJson);

    for (int i{}; i < prjData.TimingData.count(); i++)
    {
        timingArr.append(static_cast<int>(prjData.TimingData.at(i)));
    }

    QJsonObject finalData;

    finalData["prjAddr"]      = prjData.prjAddr;
    finalData["prjDir"]       = prjData.prjDir;
    finalData["dataDir"]      = prjData.dataDir;
    finalData["isLoaded"]     = prjData.isLoaded;
    finalData["isSaved"]      = prjData.isSaved;
    finalData["isSaveNeeded"] = prjData.isSaveNeeded;
    finalData["isGenerated"]  = prjData.isGenerated;
    finalData["prjBand"]      = prjData.radarBand;
    finalData["prjMode"]      = prjData.radarMode;
    finalData["prjFreq"]      = prjData.radarFreq;
    finalData["TimingData"]   = timingArr;
    finalData["radarData"]    = dataArr;
    finalData["prjName"]      = prjData.prjName;
    finalData["srate"]        = static_cast<int>(prjData.srate);


    //---- Creating Iq file -----------------
    if(saveProject(finalData))
    {
        QStringList arguments;
        arguments << matlabRunTimePath << finalData["prjAddr"].toString();
        connect(processIq,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(createIqIsDone(int,QProcess::ExitStatus )));
        processIq->start(program,arguments);
        showLoadingBar(true);
    }

    //--------------Remove existing file--------------------
    removeIqFile(finalData);
}


void SattarUtils::simplifiedIQ(std::vector <int16_t> &vecInt)
{
    std::ofstream iqFile("./sattar/simlifiedIqFile.txt");

    for (const auto &e:vecInt)
    {
        iqFile << e <<"\n";
    }

}

void SattarUtils::normalIq(std::vector <int16_t> &vecInt)
{
    std::ofstream iqFile2("./sattar/"+radarName.toStdString()+".txt");

    iqFile2 << "TEXT\n";

    int p = normalSample;
    for (int i{}; i < normalSample ; i += sampleStep)
    {
        //--first var
        double val1 = static_cast<double>(vecInt[i]) / maxNormal ;
        std::stringstream tmp;
        tmp << std::setprecision(15) << std::fixed << val1;
        double tmpVal1 = stod(tmp.str());

        tmp.str(std::string());

        //--second var
        double val2 = static_cast<double>(vecInt[i+1]) / maxNormal ;
        tmp << std::setprecision(15) << std::fixed << val2;
        double tmpVal2 = stod(tmp.str());
        tmp.str(std::string());

        iqFile2 << tmpVal2 << "," <<tmpVal1 << "\n";
    }

    filePath = "./sattar/"+QString(radarName)+".txt";
    //    iqFile2.close();
}

void SattarUtils::removeIqFile( QJsonObject &finalData)
{
    fileAdress = finalData["prjDir"].toString()+"Data1.dat";
    QByteArray ba = fileAdress.toLocal8Bit();
    const char *c_str2 = ba.data();
    std::ifstream existFile(fileAdress.toStdString());
    if(existFile) std::remove(c_str2);
}

void SattarUtils::minimizedIqFile()
{
    std::ifstream existFile(fileAdress.toStdString());

    if(existFile)
    {
        int16_t data;
        std::vector <int16_t> vecInt;

        for (int i{};i < normalSample; i++)
        {
            existFile.read((char*)&data, sizeof (int16_t));
            vecInt.push_back(data);
        }

        normalIq(vecInt);

//        loadFileToCart("sattar/"+radarName, 0,"sattar");
        simplifiedIQ(vecInt);
    }

    else
    {
        msgFunction("IQ file does not exist");
    }
}



//execut when creating iq is finished
void SattarUtils::createIqIsDone(int finishProcess,QProcess::ExitStatus z)
{
    Q_UNUSED(z);

    if(finishProcess == 0)
    {
        showLoadingBar(false);
        minimizedIqFile();

        if(isSendToCard)
        {
            emit loadSattarFileToCart("sattar/"+radarName+".txt", 0,"sattar");
        }

        msgFunction("File created successfully.");



    }

    else
    {
        showLoadingBar(false);
        msgFunction("Something went wrong. \n try again.");
    }
}



void SattarUtils::showLoadingBar(bool status)
{
    if (status)
    {
        newProg->setMaximum(0);
        newProg->setMinimum(0);
        newProg->setValue(0);

        newProg->setVisible(true);
        s->addWidget(newProg);
        d->setLayout(s);
        d->move(300,300);
        d->show();
    }
    else
    {
        newProg->setVisible(false);
        d->setVisible(false);
    }
}



bool SattarUtils::saveProject(QJsonObject & jsonVal)
{

    //    QString fileName = QFileDialog::getSaveFileName();
    QString fileName = QDir::currentPath()+"/jsonIQ";

    jsonVal["prjAddr"] = fileName;

    auto dataDir = fileName.split("/");
    jsonVal["prjName"] = dataDir[dataDir.length()-1];
    dataDir.removeAt(dataDir.length()-1);

    QString iqPath = dataDir.join("/");
    jsonVal["prjDir"]  = iqPath+"/";
    jsonVal["dataDir"] = iqPath+"/";

    if (fileName.isEmpty()) return false;

    else
    {
        QFile file(fileName);

        if (!file.open(QIODevice::WriteOnly))
        {
            msgFunction("Unable to open file");
            return false;
        }

        QJsonDocument saveDoc(jsonVal);
        QByteArray doc = saveDoc.toJson();
        file.write(doc);
        return true;
    }
    return false;
}

void SattarUtils::colorState(colorStatus mode,bool state)
{
    switch (mode)
    {

    case pulsePage:
        isBtnColorGreen[0] = state;
        break;


    case priPage:
        isBtnColorGreen[1] = state;
        break;


    case pwPage:
        isBtnColorGreen[2] = state;
        break;


    case frqPage:
        isBtnColorGreen[3] = state;
        break;


    case modulPage:
        //        isBtnColorGreen[4] = state;
        break;

    default:
        break;
    }
}


void SattarUtils::showSelectedRadar(modulSpace::modulDataStruct         & modulVal,
                                    radarSpace::radarDataStruct         & radarVal,
                                    frqSpace::frqDataStruct             & frqVal  ,
                                    priSpace::priDataStruct             & priVal  ,
                                    pwSpace::pwDataStruct               & pwVal   ,
                                    spureSpace::spureStruct             & spureVal,
                                    projectDataSpace::projectDataStruct & prjData)
{


    for (uint i{}; i < sizeof (isBtnColorGreen) / sizeof (bool); i++)
    {
        if (isBtnColorGreen[i] == false)
        {

            msgFunction("Please confirm all tabs.");
            return;
        }
    }


    QString msgFunction("1) Radar Name: "            + radarVal.radarName                      +'\n'+
                        "2) Radar ShiftTime: "       + QString::number(radarVal.shiftTime)     +'\n'+
                        "3) Priority: "              + QString::number(radarVal.radarPriority) +'\n'+
                        "4) Miss Pulse: "            + QString::number(radarVal.missPulse)     +'\n'+
                        "5) Spur: "                  + QString::number(radarVal.spurPercent)   +'\n'+
                        "6) PW Mode: "               + pwVal.Type                              +'\n'+
                        "7) PW Time: "               + QString::number(pwVal.pwConstant)       +'\n'+
                        "8) PRI Mode: "              + priVal.type                             +'\n'+
                        "9) PRI Frequency: "         + priVal.priBase                          +'\n'+
                        "10) Frequency Modulation: " + frqVal.frqType                          +'\n'+
                        "11) Base Frequency: "       + QString::number(frqVal.baseFrq)         +'\n'+
                        "12) Modulation Mode: "      + modulVal.type                           +'\n'+
                        "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
                        );


    QMessageBox msgBox;
    msgBox.setText("Data is completed.");
    QString msg= "Do you want to generate iq data? ";
    msgBox.setInformativeText(msgFunction+"\n\n"+ msg);

    QAbstractButton * sendToCart   = msgBox.addButton(tr("save and send") , QMessageBox::ActionRole);
    QAbstractButton * save         = msgBox.addButton(tr("save")          , QMessageBox::ActionRole);
    QAbstractButton * cancel       = msgBox.addButton(tr("cancel")        , QMessageBox::ActionRole);
    //    msgBox.setStandardButtons( QMessageBox::Cancel);
    //    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();


    if (msgBox.clickedButton() == sendToCart)
    {
        isSendToCard = true;
        generateRadarData(radarVal,modulVal,spureVal,priVal,frqVal,pwVal,prjData,0);

    }
    else if (msgBox.clickedButton() == save)
    {
        isSendToCard = false;
        generateRadarData(radarVal,modulVal,spureVal,priVal,frqVal,pwVal,prjData,1);
    }
    else if (msgBox.clickedButton() == cancel)
    {
        isSendToCard = false;
        return;
    }


    //    switch (ret) {
    //    case QMessageBox::Ok:
    //    {


    //    }
    //        break;

    //    case QMessageBox::Cancel:
    //        // Cancel was clicked
    //        break;
    //    default:
    //        // should never be reached
    //        break;
    //    }


}

bool SattarUtils::setMaxPulseCount(uint count, radarSpace::radarDataStruct &radarData)
{
    bool returnValue{false};

    if(radarData.maxPulseCount == 1 && count >=1)
    {
        radarData.maxPulseCount = count;
        returnValue =  true;
    }

    else if(radarData.maxPulseCount == count)
    {
        returnValue =  true;
    }

    else if(radarData.maxPulseCount > 1 && count == 1)
    {
        returnValue = true;
    }
    else
    {
//        msgFunction("pw pulse count and PRI/PRF pulse count must be equal.");
//        returnValue = false;
    }

    return  returnValue;
}

int SattarUtils::getTotalPulseCount(radarSpace::radarDataStruct &radarData)
{
    int returnValue{-1};
    if(radarData.maxPulseCount >= 1)
    {
        returnValue = radarData.maxPulseCount;
    }
    else
    {
//        msgFunction("pw pulse count and PRI/PRF pulse count must be equal.3");
    }

    return  returnValue;
}

bool SattarUtils::checkError(priSpace::priDataStruct &priData, pwSpace::pwDataStruct &pwData)
{

    if(pwData.countPw == static_cast<uint>(priData.pri.count()))
    {

        for(uint i{}; i < pwData.countPw; i++)
        {
            if(pwData.pw[i] >= priData.pri[i])
            {
                return false;
            }
        }
        return true;
    }

    else
    {
        return false;
    }
}




bool SattarUtils::spureGenerateDate(spureSpace::spureStruct &spurData,
                                    modulSpace::modulDataStruct& modulDataVal,
                                    radarSpace::radarDataStruct& radarDataVal,
                                    frqSpace::frqDataStruct    & frqDataVal  ,
                                    priSpace::priDataStruct    & priDataVal  ,
                                    pwSpace::pwDataStruct      & pwDataVal    )
{

    if(radarDataVal.spurPercent > 0)
    {
        generateSpureData(pwDataVal,spurData);
        generateSpureData(pwDataVal.pw[0], priDataVal,spurData);
        generateSpureData(frqDataVal,spurData);
        generateSpureData(pwDataVal.pw[0],frqDataVal.frq[0], modulDataVal,spurData);
        radarDataVal.spurCount = 1;
    }
    return true;
}

bool SattarUtils::generateSpureData(pwSpace::pwDataStruct &pwData,spureSpace::spureStruct &spurData)
{
    spurData.pwSpure.pw.clear();
    spurData.pwSpure.pw.append(10000000);
    spurData.pwSpure.countPw = 1;
    spurData.pwSpure.maxPw = pwData.pwConstant;
    spurData.pwSpure.minPw = pwData.pwConstant;
    return true;
}

bool SattarUtils::generateSpureData(uint &pwVal, priSpace::priDataStruct & priData,spureSpace::spureStruct &spurData)
{

    spurData.priSpure.priBase = "500 Hz";
    spurData.priSpure.pri.clear();
    spurData.priSpure.pri.append(2000000);
    spurData.priSpure.CountPri = 1;
    spurData.priSpure.maxPri = 2000000;
    spurData.priSpure.minPri = 2000000;
    return true;

}

bool SattarUtils::generateSpureData(uint &pw,float &freq, modulSpace::modulDataStruct &modulData,spureSpace::spureStruct &spurData)
{


    if(spurData.modulSpure.type == modulSpace::modeTypeEnum::xFM){

        if (spurData.modulSpure.xfmTargetFrq <= freq)
        {
            spurData.modulSpure.xfmTargetFrq = QRandomGenerator::global()->bounded( freq, 500);
        }

        spurData.modulSpure.xfmTargetFrq = QRandomGenerator::global()->bounded( 10, 50) * 100;

        if(QRandomGenerator::global()->bounded( 1, 2) == 1)
        {
            spurData.modulSpure.xFMType  = modulSpace::xfmTypeEnum::LFM;
            spurData.modulSpure.LFMSweetDirection = modulSpace::lfmSweepDirectionEnum::UniPolar;
        }
        else{
            spurData.modulSpure.xFMType = modulSpace::xfmTypeEnum::NLFM;
            spurData.modulSpure.LFMSweetDirection = modulSpace::lfmSweepDirectionEnum::Bipolar;
        }
    }


    else if (spurData.modulSpure.type == modulSpace::modeTypeEnum::FSK)
    {
        spurData.modulSpure.FSKType = modulSpace::fskTypeEnum::Simple1;
        spurData.modulSpure.fskDeltaF = 10;
        spurData.modulSpure.fskSimpleTB = QRandomGenerator::global()->bounded( 5, 50) * 100;
        spurData.modulSpure.fskSimpleBitInfo = "1,1,0,0,1,0,1";
    }



    else if (spurData.modulSpure.type ==  modulSpace::modeTypeEnum::BPSK)
    {
        spurData.modulSpure.bpsKType = modulSpace::bpskTypeEnum::Barker;
        spurData.modulSpure.bpskTB = QRandomGenerator::global()->bounded( 5, 50) * 100;
        spurData.modulSpure.bpskBarkerCode = 7;
    }



    else if (spurData.modulSpure.type == modulSpace::modeTypeEnum::QPSK)
    {
        spurData.modulSpure.qpskType = modulSpace::QpskTypeEnum::Custom1;
        spurData.modulSpure.qpskTB = QRandomGenerator::global()->bounded( 50, 50) * 100;
        spurData.modulSpure.qpskCustomCode = "00,11,10,01";
    }

    else if (spurData.modulSpure.type == modulSpace::modeTypeEnum::Simple)
    {

    }

    return true;
}

bool SattarUtils::generateSpureData(frqSpace::frqDataStruct &frqData,spureSpace::spureStruct &spurData)
{
    spurData.frqSpure.baseFrq = static_cast<float>(QRandomGenerator::global()->bounded( 100, 450));
    spurData.frqSpure.frq.clear();
    spurData.frqSpure.frq.append(frqData.baseFrq);
    spurData.frqSpure.minFrq = frqData.baseFrq;
    spurData.frqSpure.maxFrq = frqData.baseFrq;
    spurData.frqSpure.countFrq = 1;
    return true;
}

int SattarUtils::regeneratePulsePRI(uint count, priSpace::priDataStruct &priVal)
{

    int  returnValue{-1};

    if(priVal.isGenerated)
    {

        if(count ==1)
        {
            returnValue =  priVal.CountPri;
        }

        else if (priVal.CountPri == count)
        {
            returnValue =  count;
        }

        else if (priVal.CountPri < count && priVal.CountPri == 1)
        {
            for(uint i{}; i < count -1; i++)
            {
                priVal.pri.append(priVal.pri[0]);
            }

            priVal.CountPri = priVal.pri.count();

            returnValue =  priVal.CountPri;
        }
    }

    else
    {
//        msgFunction("pw pulse count and PRI/PRF pulse count must be equal.2");
    }

    return   returnValue ;
}

int SattarUtils::regeneratePulsePW(uint count, pwSpace::pwDataStruct &pwData)
{
    int returnValue{-1};

    if(pwData.isGenerated)
    {
        if(count == 1)  returnValue =  pwData.countPw;

        else if(pwData.countPw == count)  returnValue =   count;

        else if (pwData.countPw ==1)
        {
            for( uint i{}; i < count -1; i++)
            {
                pwData.pw.append(pwData.pw[0]);
            }
            pwData.countPw = count;
            returnValue =  count;
        }
    }
    else
    {
//        msgFunction("pw pulse count and PRI/PRF pulse count must be equal.4");
    }

    return returnValue;
}


