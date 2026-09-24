//#include "calculateplotdata.h"

//calculatePlotData::calculatePlotData(QObject *parent)
//{

//}


//void calculatePlotData::run()
//{

//}




//void calculatePlotData::ValueSlot( QString * val, int mode)
//{
//    if (ui->tabWidget->currentIndex() != 1 ) return;

//    double value {val->toDouble()};
//    switch (mode) {
//    case 1://FRQ
//    {
//        frqMax = maximumFinder(&value, &frqMax);
//        //        frqCnt = plottingValues(&value, ui->wigFRQ, &xFrq, &yFrq,(centerFrq - centerFrq * 0.8), (frqMax+ frqMax * 0.3) ,frqCnt);
//        frqCnt = plottingValues(&value, ui->wigFRQ, &xFrq, &yFrq,(frqMax- frqMax * 0.03), (frqMax+ frqMax * 0.003) ,frqCnt);
//        fillHistoramArray(mode, ui->wigFRQhisto, &value, frqCnt);
//        frqCnt++;
//    }
//        break;

//    case 2://PAPeak
//    {
//        PAPeakCnt++;
//        PAPeakMax = maximumFinder(&value, &PAPeakMax);
//        PAPeakCnt = plottingValues(&value, ui->wigPAPEAK, &xPAPEAK, &yPAPEAK,-1,(PAPeakMax + PAPeakMax* 0.3),PAPeakCnt);
//    }
//        break;


//    case 3://PASum
//    {
//        PASumMax = maximumFinder(&value, &PASumMax);
//        PASumCnt = plottingValues(&value, ui->wigPASUM , &xPASUM, &yPASUM,-1,(PASumMax + PASumMax* 0.2),PASumCnt);
//        fillHistoramArray(mode, ui->wigPASumhisto, &value, PASumCnt);
//        PASumCnt++;
//    }
//        break;


//    case 4://PRI
//    {
//        PRIMax = maximumFinder(&value, &PRIMax);
//        PRIMin = minimumFinder(&value, &PRIMin);
//        PRICnt = plottingValues(&value, ui->wigPRI, &xPRI, &yPRI,(PRIMin - PRIMin* 0.2),(PRIMax + PRIMax* 0.2),PRICnt);
//        fillHistoramArray(mode, ui->wigPRIhisto, &value, PRICnt);
//        PRICnt++;
//    }
//        break;


//    case 5://PW1
//    {
//        PW1Max = maximumFinder(&value, &PW1Max);
//        PW1Cnt = plottingValues(&value, ui->wigPW1,&xPW1,& yPW1,-1,(PW1Max + PW1Max* 0.2),PW1Cnt);
//        fillHistoramArray(mode, ui->wigPWhisto, &value, PW1Cnt);
//        PW1Cnt++;
//    }
//        break;


//    case 6://PW2
//    {
//        PW2Cnt++;
//        PW2Max =maximumFinder(&value, &PW2Max);
//        PW2Cnt = plottingValues(&value, ui->wigPW2, &xPW2, &yPW2,-1,(PW2Max + PW2Max* 0.2),PW2Cnt);

//    }
//        break;


//    case 7://TOA
//    {
//        TOACnt++;
//        TOAMax = maximumFinder(&value, &TOAMax);
//        TOACnt = plottingValues(&value, ui->wigTOA , &xTOA, &yTOA,-1,(TOAMax + TOAMax* 0.2),TOACnt);

//    }
//        break;


//    case 8://Channel
//    {
//        ChannelCnt++;
//        ChannelMax = maximumFinder(&value, &ChannelMax);
//        ChannelCnt = plottingValues(&value, ui->wigCHANNEL, &xCHANNEL, &yCHANNEL,-1,(ChannelMax  +ChannelMax* 0.2),ChannelCnt);

//    }
//        break;


//    default:
//        break;
//    }
//}



//int calculatePlotData::plottingValues( const double * val,QCustomPlot * plot,QVector<double> * x,QVector<double> * y, double min, double max,int count)
//{
//    if (count < 100)
//    {
//        x->append(count);
//        y->append(*val);
//        return count;
//    }

//    else
//    {
//        plot->graph(0)->setData(*x, *y);
//        plot->xAxis->setRange(0,x->size());
//        plot->yAxis->setRange(min, max);
//        plot->replot();

//        x->clear();
//        y->clear();
//        count = -1;
//        return count;
//    }
//}


//void calculatePlotData::plotHistogram(QCustomPlot * plot, std::unordered_map<double, double> * histogramData)
//{

//    QVector<double> data;
//    QVector<double> lineData;

//    for (auto it = histogramData->begin(); it != histogramData->end(); ++it)
//    {
//        if (it->second > 2){
//            data.append(it->first);
//            lineData.append(it->second);
//        }

//        if (xMax <= it->second)
//        {
//            yMax = it->first; // maximum data for y axios
//            xMax = it->second; // maximun data for x axios
//        }
//    }

//    auto minmax1 = std::minmax_element(data.begin(), data.end());
//    double min_YAxis = *minmax1.first;
//    double max_YAxis = *minmax1.second;

//    auto minmax2 = std::minmax_element(lineData.begin(), lineData.end());
//    double min_XAxis = *minmax2.first;
//    double max_XAxis = *minmax2.second;

//    plot->xAxis->setRange(min_XAxis- (min_XAxis * 0.5), max_XAxis + (max_XAxis * 0.8));
//    plot->yAxis->setRange(min_YAxis - (min_YAxis * 0.001) , max_YAxis + (max_YAxis * 0.001));

//    QCPBars *bars = new QCPBars(plot->yAxis,plot->xAxis);

//    // add the text label at the top:
//    QCPItemText *textLabel = new QCPItemText(plot);
//    textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignRight);
//    textLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
//    textLabel->position->setCoords(1,0.05);
//    textLabel->setText("val: "+QString::number(yMax)+"\nnum: "+QString::number(xMax));
//    textLabel->setFont(QFont(font().family(), 15));
//    //    textLabel->setPen(QPen(QColor(1, 133, 17)));
//    textLabel->setColor(Qt::green);

//    bars->setData(data,lineData);
//    bars->setWidth(0.8);
//    bars->setPen(QPen(Qt::yellow, 2));
//    //    bars->setBrush(Qt:: '#d6fbff' );

//    plot->replot();
//    plot->clearItems();
//    plot->clearGraphs();
//    plot->removePlottable(bars);
//    //    plot->replot();
//    xMax =0;
//    xMin=9999999999;
//    yMax=0;
//    yMin=0;
//}

//void calculatePlotData::fillHistoramArray(int type, QCustomPlot * plot, double * val,int cnt)
//{
//    switch (type)
//    {

//    case 1://FRQ
//    {
//        if(cnt == -1)
//        {
//            plotHistogram(plot, &frqMap);
//            frqMap.clear();
//        }
//        else {

//            frqMap[*val]++;

//        }
//    }
//        break;
//        //    case 2://PAPeak
//        //    {
//        //    }
//        //        break;


//    case 3://PASum
//    {
//        if(cnt == -1)
//        {
//            plotHistogram(plot, &piSumMap);
//            piSumMap.clear();
//        }
//        else {
//            piSumMap[*val]++;
//        }

//    }
//        break;


//    case 4://PRI
//    {
//        if(cnt == -1)
//        {
//            plotHistogram(plot, &priMap);
//            priMap.clear();
//        }
//        else {
//            priMap[*val]++;
//        }
//    }
//        break;


//    case 5://PW1
//    {


//        if(cnt == -1)
//        {
//            plotHistogram(plot, &pwMap);
//            pwMap.clear();
//        }
//        else {
//            pwMap[*val]++;
//        }
//    }
//        break;


//        //    case 6://PW2
//        //    {

//        //    }
//        //        break;


//        //    case 7://TOA
//        //    {

//        //    }
//        //        break;


//        //    case 8://Channel
//        //    {

//        //    }
//        //        break;


//    default:
//        break;
//    }
//}



//double calculatePlotData::maximumFinder(double * newVal,double * max)
//{

//    if(*newVal > *max)
//    {
//        max = newVal;
//    }
//    return *max;
//}

//double calculatePlotData::minimumFinder(double * newVal,double * min)
//{
//    if(*newVal < *min)
//    {
//        min = newVal;
//    }

//    return *min;
//}
