#include "QGLchart.h"
#include <iostream>
#include <QOpenGLDebugMessage>
#include <QMouseEvent>
#include <QHelpEvent>
// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H


QGLchart::QGLchart(QWidget *parent)
    : QOpenGLWidget(parent)
{

    SetupUi(this);
}

void QGLchart::SetupUi(QWidget *parent)
{
    qRegisterMetaType<QPair<QVector4D, QString> >("QPair<QVector4D, QString>");
    m_zoomState = false;
    m_SyncedZoom = false;
    m_background.Visibility = true;
    m_background.Roundness = true;
    m_background.DropShadowEnable = false;
    m_background.Brush.setColor(Qt::black);

    ctrlKeyPressed = false;

    /*--- Zoom Variables ---*/
    m_zoomState = false;
    m_zoomAreaSelected = false;
    m_manualZoomType = false;
    m_verticalUpperRangeMargin = 0;
    m_verticalLowerRangeMargin = 0;
    m_scatterUpperRangeMargin = 0;
    m_scatterLowerRangeMargin = 0;

    /*--- Masking Variables ---*/
    m_maskingEnabled = false;
    m_maskingModifiable = false;
    m_maskingColor = QColor(Qt::white);
    m_initialMaskingLowerEdge = 0;
    m_initialMaskingLowerEdge = 0;

    /*--- Marker Variables ---*/
    m_markerEnabled = false;
    m_markerColor = Qt::white;
    m_currentMarkerIndex = -1;

    /*--- Threshold Variables ---*/
    m_thresholdPoints.clear();
    m_thresholdEnabled = false;
    m_thresholdModifiable = false;
    m_thresholdInitialLevel = 0;
    m_modifiableSectionsNumber = 16;
    m_thresholdType = ThresholdType::Rectangular;
    m_thresholdColor = QColor(Qt::red);

    /*--- Title Related Variables ---*/
    m_titleText = "";
    m_titleBrush.setColor(Qt::white);

    /*--- Label Position Variables ---*/
    m_verticalLebelRelPos = QVector2D(0.025, 0.1);
    m_applyFineTuning = true;

    m_manualHorzScalePosEnabled = false;

    /*--- Horizontal axis variables ---*/
    m_hNumberCount = 8;
    m_ChartVerticalRangeOffset = QVector2D(0.0, 0.0);

    /*--- ToolTip Variables ---*/
    m_ToolTipEnabled = false;
    m_maxAcceptableDistance = QVector2D(0, 0);

    /*--- yAxis2 Variables ---*/
    yAxis2 = new QCustomAxis(this);
    yAxis2->setVisible(false);

    m_areaSelectionState = false;

    m_chartType = Spectrum;
    changeWaterfall = false;
    changeHorizantalWaterfall = false;
    m_verticalChart = false;
    numberOfLine = 0;

    m_HorizantalNumberPrecision = 2;
    m_verticalNumberPrecision = 1;

    m_newSpectrumDataReceived = false;
}

/****************************************************************************************/
/*                            Mouse and Keyboard events                                 */
/****************************************************************************************/

void QGLchart::mouseDoubleClickEvent(QMouseEvent *ev)
{
    m_zoomAreaSelected = false;

    if (m_SyncedZoom)
    {
        QVector4D zoomOut;
        if(!m_addedSeriesData.isEmpty())
            emit(selectedZoomArea(QPair<QVector4D, QString>(zoomOut, m_addedSeriesData.at(0).getSeriesName())));
    }
}

void QGLchart::mousePressEvent(QMouseEvent *ev)
{
    if ((ev->button() == Qt::LeftButton) && (m_zoomState == true))
    {
        if (m_drawRegion.contains(ev->pos()))
        {
            //m_firstMousePos = ev->pos() - m_drawRegion.topLeft();
            m_firstMousePos = ev->pos();
            m_mouseMoved = false;
        }
    }

    if ((ev->button() == Qt::LeftButton) && (m_markerEnabled == true))
    {
        if (m_drawRegion.contains(ev->pos()))
        {
            QPointF mousePos = translateMousePosToDataRange(ev->pos() - m_drawRegion.topLeft());
            if (!mousePos.isNull())
            {
                QVector2D nearData = infoOfNearestData(mousePos, 0);
                if (m_markerCenterPoint.isEmpty())
                    m_markerCenterPoint.append(nearData);
                else
                {
                    if (ctrlKeyPressed)
                        m_markerCenterPoint.append(nearData);
                    else
                    {
                        m_markerCenterPoint.removeLast();
                        m_markerCenterPoint.append(nearData);
                    }
                }
                emit selectedMarkerData(nearData);
                /*QPoint currentDataPos = translateDataInfoToMousePos(nearData);
                QToolTip::showText(currentDataPos + m_drawRegion.topLeft(), QString::number(nearData.y(), 'f', 3), this, m_drawRegion, 3500);*/
            }
        }
    }

    if ((ev->button() == Qt::LeftButton) && (m_areaSelectionState == true))
    {
        if (m_drawRegion.contains(ev->pos()))
        {
            //m_firstMousePos = ev->pos() - m_drawRegion.topLeft();
            m_firstMousePos = ev->pos();
            m_mouseMoved = false;
        }
    }

    if ((ev->button() == Qt::LeftButton) && (m_thresholdModifiable == true))
    {
        if (m_drawRegion.contains(ev->pos()))
        {
            QPointF mousePos = translateMousePosToDataRange(ev->pos() - m_drawRegion.topLeft());
            int nearIndex = indexOfNearestThrPoint(mousePos);
            if (nearIndex > -1)
            {
                m_PreviousPressPos = mousePos;
                m_selectedIndex = nearIndex;
                m_endOfThresholdModify = false;
            }
        }
    }

    if ((ev->button() == Qt::LeftButton) && (m_maskingEnabled) && (m_maskingModifiable))
    {
        if (m_drawRegion.contains(ev->pos()) && (!m_maskingCenterPoints.isEmpty()))
        {
            QCursor newCursor;
            // For now a single region is considered
            QPointF maskingMousePos = translateMousePosToDataRange(ev->pos() - m_drawRegion.topLeft());
            m_firstMaskingPressPos = maskingMousePos;
            double halfWidth = determineHalfRange();

            /* ********************************************************
             * changed By mohamadi:
             * - added 'for'
             * - added swap things(cause haven't to change other things)
             * *********************************************************/

            QVector2D currentMaskingRegion;
            QVector2D swapMaskingRegion;
            for(int i = 0; i < m_maskingCenterPoints.length(); i++)
            {
                currentMaskingRegion = m_maskingCenterPoints.at(i);
                if ((maskingMousePos.x() >= (currentMaskingRegion.x() - (currentMaskingRegion.y() * 0.5) + halfWidth)) && (maskingMousePos.x() <= (currentMaskingRegion.x() + (currentMaskingRegion.y() * 0.5) - halfWidth)))
                {
                    // -------------------- added by mohamadi: ----------------
                    swapMaskingRegion = m_maskingCenterPoints[0];
                    m_maskingCenterPoints[0] = currentMaskingRegion;
                    m_maskingCenterPoints[i] = swapMaskingRegion;
                    // --------------------------------------------------------

                    newCursor.setShape(Qt::CursorShape::SizeAllCursor);
                    this->setCursor(newCursor);

                    m_MaskingModifyType = MaskingModifyType::centerRegion;
                    m_endOfMaskingModify = false;
                }

                else if ((maskingMousePos.x() < (currentMaskingRegion.x() - (currentMaskingRegion.y() * 0.5) + halfWidth)) && (maskingMousePos.x() > (currentMaskingRegion.x() - (currentMaskingRegion.y() * 0.5) - halfWidth)))
                {
                    // -------------------- added by mohamadi: ----------------
                    swapMaskingRegion = m_maskingCenterPoints[0];
                    m_maskingCenterPoints[0] = currentMaskingRegion;
                    m_maskingCenterPoints[i] = swapMaskingRegion;
                    // --------------------------------------------------------

                    newCursor.setShape(Qt::CursorShape::SizeHorCursor);
                    this->setCursor(newCursor);

                    m_MaskingModifyType = MaskingModifyType::leftEdge;
                    m_initialMaskingLowerEdge = currentMaskingRegion.x() - (currentMaskingRegion.y() * 0.5);
                    m_initialMaskingHigherEdge = currentMaskingRegion.x() + (currentMaskingRegion.y() * 0.5);
                    m_endOfMaskingModify = false;
                }
                else if ((maskingMousePos.x() > (currentMaskingRegion.x() + (currentMaskingRegion.y() * 0.5) - halfWidth)) && (maskingMousePos.x() < (currentMaskingRegion.x() + (currentMaskingRegion.y() * 0.5) + halfWidth)))
                {
                    // -------------------- added by mohamadi: ----------------
                    swapMaskingRegion = m_maskingCenterPoints[0];
                    m_maskingCenterPoints[0] = currentMaskingRegion;
                    m_maskingCenterPoints[i] = swapMaskingRegion;
                    // --------------------------------------------------------

                    newCursor.setShape(Qt::CursorShape::SizeHorCursor);
                    this->setCursor(newCursor);

                    m_MaskingModifyType = MaskingModifyType::rightEdge;
                    m_initialMaskingLowerEdge = currentMaskingRegion.x() - (currentMaskingRegion.y() * 0.5);
                    m_initialMaskingHigherEdge = currentMaskingRegion.x() + (currentMaskingRegion.y() * 0.5);
                    m_endOfMaskingModify = false;
                }
            }
        }
    }
}

void QGLchart::mouseMoveEvent(QMouseEvent *ev)
{
    if ((ev->buttons() & Qt::LeftButton) && (m_zoomState == true))
    {
        if (m_drawRegion.contains(ev->pos()))
        {
            if (!m_manualZoomType)
                m_zoomType = determineZoomType(m_firstMousePos, ev->pos());
            determineZoomAreaRectangle(m_firstMousePos - m_drawRegion.topLeft(), ev->pos() - m_drawRegion.topLeft(), m_zoomType);
        }
        else
        {
            QPoint projectedPoint = projectPointToDrawRegion(ev->pos());
            if (!m_manualZoomType)
                m_zoomType = determineZoomType(m_firstMousePos, projectedPoint);
            determineZoomAreaRectangle(m_firstMousePos - m_drawRegion.topLeft(), projectedPoint - m_drawRegion.topLeft(), m_zoomType);
        }
        m_mouseMoved = true;
    }

    if ((ev->buttons() & Qt::LeftButton) && (m_areaSelectionState == true))
    {
        if (m_drawRegion.contains(ev->pos()))
        {
            m_zoomType = HorizantalZoom;
            determineZoomAreaRectangle(m_firstMousePos - m_drawRegion.topLeft(), ev->pos() - m_drawRegion.topLeft(), m_zoomType);
        }
        else
        {
            QPoint projectedPoint = projectPointToDrawRegion(ev->pos());
            m_zoomType = HorizantalZoom;
            determineZoomAreaRectangle(m_firstMousePos - m_drawRegion.topLeft(), projectedPoint - m_drawRegion.topLeft(), m_zoomType);
        }
        m_mouseMoved = true;
    }

    if ((ev->buttons() & Qt::LeftButton) && (m_thresholdModifiable == true) && (!m_endOfThresholdModify))
    {
        if (m_drawRegion.contains(ev->pos()))
        {
            QPointF currentMousePos = translateMousePosToDataRange(ev->pos() - m_drawRegion.topLeft());
            float diff = currentMousePos.y() - m_PreviousPressPos.y();
            m_thresholdPoints[m_selectedIndex].setY(m_thresholdPoints[m_selectedIndex].y() + diff);
            m_PreviousPressPos = currentMousePos;
        }
        else
        {
            QPoint projectedPoint = projectPointToDrawRegion(ev->pos());
            QPointF currentMousePos = translateMousePosToDataRange(projectedPoint - m_drawRegion.topLeft());
            float diff = currentMousePos.y() - m_PreviousPressPos.y();
            m_thresholdPoints[m_selectedIndex].setY(m_thresholdPoints[m_selectedIndex].y() + diff);
            m_PreviousPressPos = currentMousePos;
        }
    }

    if ((ev->buttons() & Qt::LeftButton) && (m_maskingEnabled) && (m_maskingModifiable) && (!m_endOfMaskingModify))
    {
        QPointF currentMaskingMousePos;
        if (m_drawRegion.contains(ev->pos()))
        {
            currentMaskingMousePos = translateMousePosToDataRange(ev->pos() - m_drawRegion.topLeft());
        }
        else
        {
            QPoint projectedPoint = projectPointToDrawRegion(ev->pos());
            currentMaskingMousePos = translateMousePosToDataRange(projectedPoint - m_drawRegion.topLeft());
        }

        float diff = currentMaskingMousePos.x() - m_firstMaskingPressPos.x();
        checkMaskingBoundaries(diff, m_MaskingModifyType);

        if (checkMaskingBoundaries(diff, m_MaskingModifyType))
        {
            if(!m_maskingCenterPoints.isEmpty())
            {
                if (m_MaskingModifyType == MaskingModifyType::centerRegion)
                {
                    m_maskingCenterPoints[0].setX(m_maskingCenterPoints.at(0).x() + diff);
                }
                else if (m_MaskingModifyType == MaskingModifyType::leftEdge)
                {
                    m_initialMaskingLowerEdge += diff;

                    m_maskingCenterPoints[0].setX((m_initialMaskingHigherEdge + m_initialMaskingLowerEdge) / 2);
                    m_maskingCenterPoints[0].setY(abs(m_initialMaskingHigherEdge - m_initialMaskingLowerEdge));
                }
                else if (m_MaskingModifyType == MaskingModifyType::rightEdge)
                {
                    m_initialMaskingHigherEdge += diff;

                    m_maskingCenterPoints[0].setX((m_initialMaskingHigherEdge + m_initialMaskingLowerEdge) / 2);
                    m_maskingCenterPoints[0].setY(abs(m_initialMaskingHigherEdge - m_initialMaskingLowerEdge));
                }
            }
        }
        m_firstMaskingPressPos = currentMaskingMousePos;
    }
}

void QGLchart::mouseReleaseEvent(QMouseEvent *ev)
{
    if ((ev->button() == Qt::LeftButton) && (m_zoomState == true) && m_drawRegion.contains(m_firstMousePos))
    {
        if (m_mouseMoved)
        {
            if (m_drawRegion.contains(ev->pos()))
            {
                if (!m_manualZoomType)
                    m_zoomType = determineZoomType(m_firstMousePos, ev->pos());
                determineZoomArea(m_firstMousePos - m_drawRegion.topLeft(), ev->pos() - m_drawRegion.topLeft(), m_zoomType);
                m_previousZoomedData.append(m_zoomedData);
                m_zoomedData = dataInZoomArea();
                m_zoomAreaSelected = true;
            }
            else
            {
                QPoint projectedPoint = projectPointToDrawRegion(ev->pos());
                if (!m_manualZoomType)
                    m_zoomType = determineZoomType(m_firstMousePos, projectedPoint);
                determineZoomArea(m_firstMousePos - m_drawRegion.topLeft(), projectedPoint - m_drawRegion.topLeft(), m_zoomType);
                m_previousZoomedData.append(m_zoomedData);
                m_zoomedData = dataInZoomArea();
                m_zoomAreaSelected = true;
            }

            if (m_SyncedZoom & (!m_addedSeriesData.isEmpty()))
            {
                emit(selectedZoomArea(QPair<QVector4D, QString>(m_zoomedData, m_addedSeriesData.at(0).getSeriesName())));
            }
        }
        else
        {
            // single click zoom function
        }
    }

    if ((ev->button() == Qt::RightButton) && (m_zoomState == true) && m_drawRegion.contains(ev->pos()))
    {
        if (!m_previousZoomedData.isEmpty())
        {
            m_zoomedData = m_previousZoomedData.last();
            m_previousZoomedData.removeLast();

            if (m_SyncedZoom & (!m_addedSeriesData.isEmpty()))
            {
                emit(selectedZoomOutArea(QPair<QVector4D, QString>(m_zoomedData, m_addedSeriesData.at(0).getSeriesName())));
            }
        }
    }

    if ((ev->button() == Qt::LeftButton) && (m_areaSelectionState == true) && m_drawRegion.contains(m_firstMousePos))
    {
        if (m_mouseMoved)
        {
            if (m_drawRegion.contains(ev->pos()))
            {
                m_zoomType = HorizantalZoom;
                determineZoomArea(m_firstMousePos - m_drawRegion.topLeft(), ev->pos() - m_drawRegion.topLeft(), m_zoomType);
                m_selectedScanData = (QVector2D)dataInZoomArea();
            }
            else
            {
                QPoint projectedPoint = projectPointToDrawRegion(ev->pos());
                m_zoomType = HorizantalZoom;
                determineZoomArea(m_firstMousePos - m_drawRegion.topLeft(), projectedPoint - m_drawRegion.topLeft(), m_zoomType);
                m_selectedScanData = (QVector2D)dataInZoomArea();
            }

            emit(scanAreaSelected(m_selectedScanData));
        }
        else
        {
            // single click zoom function
        }
    }

    if ((ev->button() == Qt::LeftButton) && (m_thresholdModifiable == true) && (!m_endOfThresholdModify))
    {
        m_endOfThresholdModify = true;
    }

    if ((ev->button() == Qt::LeftButton) && (m_maskingEnabled) && (m_maskingModifiable) && (!m_endOfMaskingModify))
    {
        if(!m_maskingCenterPoints.isEmpty())
        {
            emit selectedMaskArea(m_maskingCenterPoints.at(0));
        }
        this->unsetCursor();
        m_endOfMaskingModify = true;
    }
    m_mouseMoved = false;

    if ((ev->button() == Qt::RightButton) && (m_zoomState == false))
    {
        /*QMenu menu;

        QAction* openAct = new QAction("Open...", this);

        menu.addAction(openAct);

        menu.addSeparator();
        menu.exec(mapToGlobal(ev->pos()));*/
    }
    QOpenGLWidget::mouseReleaseEvent(ev);
}

bool QGLchart::event(QEvent *event)
{
    if ((event->type() == QEvent::ToolTip) && (m_ToolTipEnabled))
    {
        QHelpEvent *helpEvent = static_cast<QHelpEvent *>(event);

        if (m_drawRegion.contains(helpEvent->pos()))
        {
            QPointF mousePos = translateMousePosToDataRange(helpEvent->pos() - m_drawRegion.topLeft());
            //QVector2D nearData = infoOfNearestData(mousePos, 0);
            if (m_verticalAxes->scaleType() == ScaleType::Logrithmic)
                mousePos.setY(qPow(10, mousePos.y()));
            QString shownText = "(" + QString::number(mousePos.x(), 'f', 3) + ", " + QString::number(mousePos.y(), 'f', 3) + ")";
//            QToolTip::showText(helpEvent->globalPos(), shownText, this, m_drawRegion, 3500);
        }
        else
        {
            QToolTip::hideText();
            event->ignore();
        }
    }

    if (event->type() == QEvent::Wheel)
    {
        if ((m_zoomState == true))
        {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
            if (m_drawRegion.contains(wheelEvent->pos()))
            {
                QPoint numDegrees = wheelEvent->angleDelta() / 8;
                m_zoomedData = wheelZoomDataRange(wheelEvent->pos() - m_drawRegion.topLeft(), numDegrees / 15);
                m_zoomAreaSelected = true;
                if (QVector4D(m_ChartHorizantalRange, m_ChartVerticalRange.x(), m_ChartVerticalRange.y()) == m_zoomedData)
                    m_zoomAreaSelected = false;
            }
            else
            {
                event->ignore();
            }
        }

        if ((m_markerEnabled == true) && (m_currentMarkerIndex > -1))
        {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
            if (m_drawRegion.contains(wheelEvent->pos()))
            {
                QPoint numDegrees = wheelEvent->angleDelta() / 8;
                QVector2D nearData = infoOfNextMarkerData(numDegrees / 15, 0);
                m_markerCenterPoint[0] = nearData;
                emit selectedMarkerData(nearData);
                /*QPoint currentDataPos = translateDataInfoToMousePos(nearData);
                QToolTip::showText(currentDataPos + m_drawRegion.topLeft(), QString::number(nearData.y(), 'f', 3), this, m_drawRegion, 3500);*/
            }
            else
            {
                event->ignore();
            }
        }
    }

    if ((event->type() == QEvent::MouseMove) && (m_maskingEnabled) && (m_maskingModifiable) && (m_endOfMaskingModify))
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (m_drawRegion.contains(mouseEvent->pos()) && (!m_maskingCenterPoints.isEmpty()))
        {
            QCursor newCursor;
            // For now a single region is considered
            QPointF maskingMousePos = translateMousePosToDataRange(mouseEvent->pos() - m_drawRegion.topLeft());

            QVector2D currentMaskingRegion = m_maskingCenterPoints.at(0);
            double halfRange = determineHalfRange();
            if ((maskingMousePos.x() >= (currentMaskingRegion.x() - (currentMaskingRegion.y() * 0.5) + halfRange)) && (maskingMousePos.x() <= (currentMaskingRegion.x() - halfRange + (currentMaskingRegion.y() * 0.5))))
            {
                newCursor.setShape(Qt::CursorShape::SizeAllCursor);
                this->setCursor(newCursor);
            }
            else if ((maskingMousePos.x() < (currentMaskingRegion.x() - (currentMaskingRegion.y() * 0.5) + halfRange)) && (maskingMousePos.x() > (currentMaskingRegion.x() - (currentMaskingRegion.y() * 0.5) - halfRange)))
            {
                newCursor.setShape(Qt::CursorShape::SizeHorCursor);
                this->setCursor(newCursor);
            }
            else if ((maskingMousePos.x() > (currentMaskingRegion.x() + (currentMaskingRegion.y() * 0.5) - halfRange)) && (maskingMousePos.x() < (currentMaskingRegion.x() + (currentMaskingRegion.y() * 0.5) + halfRange)))
            {
                newCursor.setShape(Qt::CursorShape::SizeHorCursor);
                this->setCursor(newCursor);
            }
            else
            {
                this->unsetCursor();
            }
        }
        else
        {
            this->unsetCursor();
            event->ignore();
        }
    }

    return QWidget::event(event);
}

void QGLchart::keyPressEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Shift)
        ctrlKeyPressed = true;
}

void QGLchart::keyReleaseEvent(QKeyEvent *ev)
{
    if (ev->key() == Qt::Key_Shift)
        ctrlKeyPressed = false;
}

/****************************************************************************************/
/*                                Add or remove Series                                  */
/****************************************************************************************/

void QGLchart::addSeries(QCustomSeries *inputSeries)
{
    m_addedSeries.append(inputSeries);
    m_addedSeriesData.append(*inputSeries);
}

void QGLchart::setSeries(QCustomSeries inputSeries)
{
    int seriesIndex = -1;

    for (int i = 0; i < m_addedSeriesData.count(); i++)
    {
        if (m_addedSeriesData[i].getSeriesName() == inputSeries.getSeriesName())
            seriesIndex = i;
    }
    if (seriesIndex > -1)
    {
        //m_addedSeriesData.replace(seriesIndex, updatedSeries);
        m_addedSeriesData[seriesIndex].addSeries(inputSeries.pointsVector());
        m_addedSeriesData[seriesIndex].setSeriesColor(inputSeries.seriesColor());
        m_addedSeriesData[seriesIndex].setVisible(inputSeries.isVisible());
        m_addedSeriesData[seriesIndex].setSeriesName(inputSeries.getSeriesName());
    }
    else
        m_addedSeriesData.append(inputSeries);

    m_newSpectrumDataReceived = true;

    if (m_chartType == Waterfall)
    {
        changeWaterfall = true;
    }

    if (m_chartType == HorizantalWaterfall)
    {
        changeHorizantalWaterfall = true;
    }

    //m_ChartHorizantalRange = getChartHorizantalRange();
    //m_ChartVerticalRange = getChartVerticalRange();
}

void QGLchart::removeAllSeries()
{
    m_addedSeries.clear();
    m_addedSeriesData.clear();
}

void QGLchart::removeSeries(QCustomSeries *inputSeries)
{
    if (!m_addedSeries.contains(inputSeries)) {
        qWarning() << QObject::tr("Can not remove series. Series not found on the chart.");
        return;
    }

    int deleteIndex = m_addedSeries.indexOf(inputSeries);
    m_addedSeries.removeAt(deleteIndex);
    m_addedSeriesData.removeAt(deleteIndex);

    emit(seriesRemoved(inputSeries));
}

void QGLchart::updateSeriesData(QCustomSeries *updatedSeries)
{
    if (!updatedSeries->getSeriesName().isEmpty())
    {
        int seriesIndex = -1;

        for (int i = 0; i < m_addedSeriesData.count(); i++)
        {
            if (m_addedSeriesData[i].getSeriesName() == updatedSeries->getSeriesName())
                seriesIndex = i;
        }
        if (seriesIndex > -1)
        {
            //m_addedSeriesData.replace(seriesIndex, *updatedSeries);
            m_addedSeriesData[seriesIndex] = *updatedSeries;
        }
        else
            addSeries(updatedSeries);
    }
    else
    {
        if (!m_addedSeries.contains(updatedSeries)) {
            addSeries(updatedSeries);
        }
        else
        {
//            m_addedSeriesData.replace(m_addedSeries.indexOf(updatedSeries), *updatedSeries);
            /* *************************************************
             * added by mohamadi:
             * for max hold, 'm_addedSeries.indexOf(updatedSeries)'
             * return main series so we should plus one the index
             * to get the max hold series and also replace not work
             * ************************************************** */
            m_addedSeriesData[m_addedSeries.indexOf(updatedSeries) + 1].removeAllPoints();
            m_addedSeriesData[m_addedSeries.indexOf(updatedSeries) + 1].addSeries(updatedSeries->pointsVector());
        }
    }

    if (m_chartType == Waterfall)
    {
        changeWaterfall = true;
    }

    if (m_chartType == HorizantalWaterfall)
    {
        changeHorizantalWaterfall = true;
    }
}

void QGLchart::updateSeriesData(QCustomSeries updatedSeries)
{
    int seriesIndex = -1;

    for (int i = 0; i < m_addedSeriesData.count(); i++)
    {
        if (m_addedSeriesData[i].getSeriesName() == updatedSeries.getSeriesName())
            seriesIndex = i;
    }
    if (seriesIndex > -1)
    {
        m_addedSeriesData[seriesIndex].addSeries(updatedSeries.pointsVector());
        m_addedSeriesData[seriesIndex].setSeriesColor(updatedSeries.seriesColor());
        m_addedSeriesData[seriesIndex].setVisible(updatedSeries.isVisible());
        m_addedSeriesData[seriesIndex].setSeriesName(updatedSeries.getSeriesName());
    }
    else
        m_addedSeriesData.append(updatedSeries);

    m_newSpectrumDataReceived = true;

    if (m_chartType == Waterfall)
    {
        changeWaterfall = true;
    }

    if (m_chartType == HorizantalWaterfall)
    {
        changeHorizantalWaterfall = true;
    }

}

QVector<QCustomSeries> QGLchart::series() const
{
//    return m_addedSeries;
    return m_addedSeriesData;
}

/****************************************************************************************/
/*                                   Add or remove Axes                                 */
/****************************************************************************************/

void QGLchart::addAxis(QCustomAxis *axis, Qt::Alignment alignment)
{
    // check already existing axes and overwrite in the same alignment
    axis->setAlignment(alignment);
    m_currentChartAxis.append(axis);
}

void QGLchart::removeAxis(QCustomAxis *axis)
{
    if (!m_currentChartAxis.contains(axis)) {
        qWarning() << QObject::tr("Can not remove axis. Axis not found on the chart.");
        return;
    }
    else
        m_currentChartAxis.removeAll(axis);
    emit(axisRemoved(axis));		// not yet connected
}

void QGLchart::createDefaultAxes()
{
    QCustomAxis *verticalAxis = new QCustomAxis(this);
    verticalAxis->setAlignment(Qt::AlignLeft);
    verticalAxis->setOrientation(Qt::Vertical);
    verticalAxis->setGridLineColor(Qt::white);
    verticalAxis->setTitleText("Power");
    verticalAxis->setLabelsText("dBm");
    //verticalAxis->setRange(35, 50);
    addAxis(verticalAxis, Qt::AlignLeft);

    QCustomAxis *horizantalAxis = new QCustomAxis(this);
    horizantalAxis->setAlignment(Qt::AlignBottom);
    horizantalAxis->setOrientation(Qt::Horizontal);
    horizantalAxis->setGridLineColor(Qt::white);
    horizantalAxis->setTitleText("Frequency");
    horizantalAxis->setLabelsText("(MHz)");
    addAxis(horizantalAxis, Qt::AlignBottom);
}

QList<QCustomAxis*> QGLchart::axes(Qt::Orientations orientation, QCustomSeries *series) const
{
    QList<QCustomAxis *> desiredAxes;

    for (int i = 0; i < m_currentChartAxis.count(); i++)
    {
        if ((m_currentChartAxis.at(i)->orientation() == orientation) && (m_currentChartAxis.at(i)->parent() == series))
            desiredAxes.append(m_currentChartAxis.at(i));
    }

    return desiredAxes;
}

QList<QCustomAxis*> QGLchart::axes(Qt::Alignment alignment) const
{
    QList<QCustomAxis *> desiredAxes;

    for (int i = 0; i < m_currentChartAxis.count(); i++)
    {
        if (m_currentChartAxis.at(i)->alignment() == alignment)
            desiredAxes.append(m_currentChartAxis.at(i));
    }

    return desiredAxes;
}

/****************************************************************************************/
/*                         Set and get Background Properties                            */
/****************************************************************************************/

void QGLchart::setBackgroundBrush(const QBrush &brush)
{
    m_background.Brush = brush;
}

QBrush QGLchart::backgroundBrush() const
{
    return m_background.Brush;
}

void QGLchart::setBackgroundPen(const QPen &pen)
{
    m_background.Pen = pen;
}

QPen QGLchart::backgroundPen() const
{
    return m_background.Pen;
}

void QGLchart::setBackgroundVisible(bool visible)
{
    m_background.Visibility = visible;
}

bool QGLchart::isBackgroundVisible() const
{
    return m_background.Visibility;
}

void QGLchart::setDropShadowEnabled(bool enabled)
{
    m_background.DropShadowEnable = enabled;
}

bool QGLchart::isDropShadowEnabled() const
{
    return m_background.DropShadowEnable;
}

/****************************************************************************************/
/*                              Title and it's properties                               */
/****************************************************************************************/

void QGLchart::setTitleText(const QString &title)
{
    m_titleText = title;
}

QString QGLchart::titleText() const
{
    return m_titleText;
}

void QGLchart::setTitleTextFont(const QFont &font)
{
    m_titleFont = font;
}

QFont QGLchart::titleTextFont() const
{
    return m_titleFont;
}

void QGLchart::setTitleTextBrush(const QBrush &brush)
{
    m_titleBrush = brush;
}

QBrush QGLchart::titleTextBrush() const
{
    return m_titleBrush;
}

/****************************************************************************************/
/*                 Set and get plot area background properties                          */
/****************************************************************************************/

QRectF QGLchart::plotArea() const
{
    return m_plotAreaBackground.Area;
}

void QGLchart::setPlotAreaBackgroundBrush(const QBrush &brush)
{
    m_plotAreaBackground.Brush = brush;
}

QBrush QGLchart::plotAreaBackgroundBrush() const
{
    return m_plotAreaBackground.Brush;
}

void QGLchart::setPlotAreaBackgroundPen(const QPen &pen)
{
    m_plotAreaBackground.Pen = pen;
}

QPen QGLchart::plotAreaBackgroundPen() const
{
    return m_plotAreaBackground.Pen;
}

void QGLchart::setPlotAreaBackgroundVisible(bool enabled)
{
    m_plotAreaBackground.Visibility = enabled;
}

bool QGLchart::isPlotAreaBackgroundVisible() const
{
    return m_plotAreaBackground.Visibility;
}

QRect QGLchart::geometry() const
{
    return m_geometry;
}

void QGLchart::setGeometry(const QRect &rect)
{
    if (m_geometry != rect)
    {
        m_geometry = rect;
        emit(plotAreaChanged(&m_geometry));
    }
}

/****************************************************************************************/
/*                           Chart Type Related functions                               */
/****************************************************************************************/

void QGLchart::setChartType(ChartType chartType)
{
    m_chartType = chartType;
    if (m_chartType == HorizantalWaterfall)
    {
        m_verticalChart = true;
    }
}

/****************************************************************************************/
/*                             Masking related functions                                */
/****************************************************************************************/

void QGLchart::enableMasking(bool enable)
{
    m_maskingEnabled = enable;
}

void QGLchart::setMaskingModifiable(bool enable)
{
    m_maskingModifiable = enable;
}

bool QGLchart::isMaskingEnabled() const
{
    return m_maskingEnabled;
}

bool QGLchart::isMaskingModifiable() const
{
    return m_maskingModifiable;
}

void QGLchart::setMaskingColor(const QColor &color)
{
    m_maskingColor = color;
}

QColor QGLchart::maskingColor() const
{
    return m_maskingColor;
}

/* The regions are assumed to be non-overlapping and ascending.
   Also the regions are set as center and bandwidth and not as start
   to end.
*/
void QGLchart::setMaskingRegions(QVector<QVector2D> maskingRegions)
{
    m_maskingCenterPoints.clear();
    m_maskingCenterPoints = maskingRegions;
}

QVector<QVector2D> QGLchart::maskingRegions() const
{
    return m_maskingCenterPoints;
}

bool QGLchart::checkMaskingBoundaries(double diff, MaskingModifyType maskingType)
{
    bool withinRange = false;
    QPointF leftEdge = translateMousePosToDataRange(QPoint(m_drawRegion.left(), m_drawRegion.center().y()) - m_drawRegion.topLeft());
    QPointF rightEdge = translateMousePosToDataRange(QPoint(m_drawRegion.right() + 1, m_drawRegion.center().y()) - m_drawRegion.topLeft());

    if (maskingType == MaskingModifyType::centerRegion)
    {
        if(!m_maskingCenterPoints.isEmpty())
        {
            if ((m_maskingCenterPoints.at(0).x() + diff + (m_maskingCenterPoints.at(0).y() / 2) <= rightEdge.x()) &&
                (m_maskingCenterPoints.at(0).x() + diff - (m_maskingCenterPoints.at(0).y() / 2) >= leftEdge.x()))
            {
                withinRange = true;
            }
        }
    }
    else if (maskingType == MaskingModifyType::leftEdge)
    {
        float centerWidth = (m_initialMaskingHigherEdge + diff + m_initialMaskingLowerEdge) / 2;
        float halfWidth = abs(m_initialMaskingHigherEdge - (m_initialMaskingLowerEdge + diff)) / 2;

        if ((centerWidth - halfWidth >= leftEdge.x()) && (centerWidth + halfWidth <= rightEdge.x()))
            withinRange = true;
    }
    else if (maskingType == MaskingModifyType::rightEdge)
    {
        float centerWidth = (m_initialMaskingHigherEdge + diff + m_initialMaskingLowerEdge) / 2;
        float halfWidth = abs(m_initialMaskingHigherEdge + diff - m_initialMaskingLowerEdge) / 2;

        if ((centerWidth - halfWidth >= leftEdge.x()) && (centerWidth + halfWidth <= rightEdge.x()))
            withinRange = true;
    }

    return withinRange;
}

double QGLchart::determineHalfRange()
{
    QVector2D rangeX;
    QVector2D rangeY;
    double halfRange;
    if (m_zoomAreaSelected)
    {
        rangeX.setX(m_zoomedData.x());
        rangeX.setY(m_zoomedData.y());
        rangeY.setX(m_zoomedData.z());
        rangeY.setY(m_zoomedData.w());
    }
    else
    {
        rangeX = m_ChartHorizantalRange;
        rangeY = m_ChartVerticalRange;
    }

    halfRange = (rangeX.y() - rangeX.x()) / 100.0;
    return halfRange;
}

/****************************************************************************************/
/*                               Marker related functions                               */
/****************************************************************************************/

void QGLchart::enableMarker(bool enable)
{
    m_markerEnabled = enable;
    this->setFocusPolicy(Qt::StrongFocus);
}

bool QGLchart::isMarkerEnabled() const
{
    return m_markerEnabled;
}

void QGLchart::setMarkerColor(const QColor &color)
{
    m_markerColor = color;
}

QColor QGLchart::markerColor() const
{
    return m_markerColor;
}

void QGLchart::initializeMarkerPosition()
{
    // not sure if its necessary
}

QVector2D QGLchart::infoOfNextMarkerData(QPoint angleDelta, int inputDataIndex)
{
    QVector<QVector2D> dataHolder = m_addedSeriesData.at(inputDataIndex).pointsVector();
    QVector2D desiredData;

    m_currentMarkerIndex += angleDelta.y();
    m_currentMarkerIndex = (m_currentMarkerIndex < 0) ? 0 : m_currentMarkerIndex;
    m_currentMarkerIndex = (m_currentMarkerIndex > (dataHolder.size() - 1)) ? (dataHolder.size() - 1) : m_currentMarkerIndex;

    desiredData = dataHolder.at(m_currentMarkerIndex);
    if ((m_verticalAxes->scaleType() == ScaleType::Logrithmic) && (!desiredData.isNull()))
        desiredData.setY(pow(10, desiredData.y()));

    return desiredData;
}

QPoint QGLchart::translateDataInfoToMousePos(QVector2D dataInfo)
{
    QVector2D rangeX;
    QVector2D rangeY;
    QPointF mousePos;
    if (m_zoomAreaSelected)
    {
        rangeX.setX(m_zoomedData.x());
        rangeX.setY(m_zoomedData.y());
        rangeY.setX(m_zoomedData.z());
        rangeY.setY(m_zoomedData.w());
    }
    else
    {
        rangeX = m_ChartHorizantalRange;
        rangeY = m_ChartVerticalRange;
    }

    mousePos.setX((dataInfo.x() - rangeX.x()) / (rangeX.y() - rangeX.x()));
    mousePos.setX(mousePos.x() * m_drawRegion.width());
    mousePos.setY((rangeY.y() - dataInfo.y()) / (rangeY.y() - rangeY.x()));
    mousePos.setY(mousePos.y() * m_drawRegion.height());

    return mousePos.toPoint();
}

void QGLchart::updateMarkerCenterPoints()
{
    int tempDataIndex = 0;
    int inputDataIndex = 0;
    QVector<QVector2D> dataHolder;

    if ((m_addedSeriesData.at(inputDataIndex).isVisible()) && (m_addedSeriesData.at(inputDataIndex).seriesSize() > 0))
    {
        dataHolder = m_addedSeriesData.at(inputDataIndex).pointsVector();

        for (int pointIndex = 0; pointIndex < m_markerCenterPoint.length(); pointIndex++)
        {
            QVector2D mousePos(m_markerCenterPoint.at(pointIndex).x(), 0.0);
            for (int i = 1; i < dataHolder.size(); i++)
            {
                if (mousePos.distanceToPoint(QVector2D(dataHolder.at(tempDataIndex).x(), 0.0)) >
                    mousePos.distanceToPoint(QVector2D(dataHolder.at(i).x(), 0.0)))
                    tempDataIndex = i;
            }
            m_markerCenterPoint[pointIndex] = dataHolder.at(tempDataIndex);

            if (m_verticalAxes->scaleType() == ScaleType::Logrithmic)
                m_markerCenterPoint[pointIndex].setY(pow(10, m_markerCenterPoint[pointIndex].y()));
        }
        emit selectedMarkerData(m_markerCenterPoint.last());
    }
}

/****************************************************************************************/
/*						       Threshold related functions								*/
/****************************************************************************************/

void QGLchart::enableThreshold(bool state)
{
    m_thresholdEnabled = state;
}

bool QGLchart::isThresholdEnabled() const
{
    return m_thresholdModifiable;
}

void QGLchart::setThresholdPoints(QVector<QVector2D> points)
{
    m_thresholdPoints = points;
}

QVector<QVector2D> QGLchart::thresholdPoints() const
{
    return m_thresholdPoints;
}

void QGLchart::setThresholdInitialLevel(double thrLevel)
{
    m_thresholdInitialLevel = thrLevel;
}

void QGLchart::setThresholdSectionNumber(int thrSectionNumber)
{
    m_modifiableSectionsNumber = thrSectionNumber;
}

void QGLchart::setThresholdType(ThresholdType thrType)
{
    m_thresholdType = thrType;
}

double QGLchart::thresholdInitialLevel() const
{
    return m_thresholdInitialLevel;
}

int	QGLchart::thresholdSectionNumber() const
{
    return m_modifiableSectionsNumber;
}

ThresholdType QGLchart::thresholdType() const
{
    return m_thresholdType;
}

void QGLchart::setThresholdColor(const QColor &color)
{
    m_thresholdColor = color;
}

QColor QGLchart::thresholdColor() const
{
    return m_thresholdColor;
}

void QGLchart::initializeThresholdPoints(ThresholdType thrType)
{
    if (!m_addedSeriesData.isEmpty())
    {
        if (m_addedSeriesData.at(0).seriesSize() > 0)
        {
            QVector2D rangeX = m_ChartHorizantalRange;
            QVector<QVector2D> desiredSeries = m_addedSeriesData.at(0).pointsVector();
            float horizantalStep = (rangeX.y() - rangeX.x()) / (m_modifiableSectionsNumber + 1);
            float meanValue = 0;
            for (int i = 0; i < desiredSeries.size(); i++)
                meanValue += desiredSeries.at(i).y();
            meanValue /= desiredSeries.size();

            for (int i = 0; i < m_modifiableSectionsNumber + 2; i++)
            {
                m_thresholdPoints.append(QVector2D(rangeX.x() + (i * horizantalStep), meanValue));
            }
        }
    }
    //if (!m_addedSeriesData.isEmpty())
    //{
    //	if (m_addedSeriesData.at(0).seriesSize() > 0)
    //	{
    //		//
    //	}
    //	else
    //	{
    //		for (int i = 0; i < m_modifiableSectionsNumber + 2; i++)
    //		{
    //			m_thresholdPoints.append(QVector2D(i, m_thresholdInitialLevel));
    //		}
    //	}
    //}
    //else
    //{
    //	for (int i = 0; i < m_modifiableSectionsNumber + 2; i++)
    //	{
    //		m_thresholdPoints.append(QVector2D(i, m_thresholdInitialLevel));
    //	}
    //}
}

void QGLchart::setThresholdModifiable(bool state)
{
    m_thresholdModifiable = state;
}

bool QGLchart::isThresholdModifiable() const
{
    return m_thresholdModifiable;
}

QPointF QGLchart::translateMousePosToDataRange(QPoint mousePos)
{
    QVector2D rangeX;
    QVector2D rangeY;
    QPointF dataZoomRanges;
    if (m_zoomAreaSelected)
    {
        rangeX.setX(m_zoomedData.x());
        rangeX.setY(m_zoomedData.y());
        rangeY.setX(m_zoomedData.z());
        rangeY.setY(m_zoomedData.w());
    }
    else
    {
        rangeX = m_ChartHorizantalRange;
        rangeY = m_ChartVerticalRange;
    }

    dataZoomRanges.setX(((float)mousePos.x() / m_drawRegion.width()) * (rangeX.y() - rangeX.x()));
    dataZoomRanges.setX(rangeX.x() + dataZoomRanges.x());
    dataZoomRanges.setY(((float)mousePos.y() / m_drawRegion.height()) * (rangeY.y() - rangeY.x()));
    dataZoomRanges.setY(rangeY.y() - dataZoomRanges.y());

    return dataZoomRanges;
}

int	QGLchart::indexOfNearestThrPoint(QPointF mousePos)
{
    int desiredIndex = -1;
    int tempIndex = 0;

    for (int i = 1; i < m_thresholdPoints.size(); i++)
    {
        if (abs(mousePos.x() - m_thresholdPoints.at(tempIndex).x()) > abs(mousePos.x() - m_thresholdPoints.at(i).x()))
            tempIndex = i;
    }
    double relDistance = (double)abs(mousePos.y() - m_thresholdPoints.at(tempIndex).y()) / abs(m_thresholdPoints.at(tempIndex).y());
    if (relDistance < 0.1)
        desiredIndex = tempIndex;
    return desiredIndex;
}

/****************************************************************************************/
/*						       Label Position Related functions							*/
/****************************************************************************************/

void QGLchart::setVerticalLabelRelPos(QVector2D relPos)
{
    m_verticalLebelRelPos = relPos;
}

QVector2D QGLchart::verticalLabelRelPos()
{
    return m_verticalLebelRelPos;
}

void QGLchart::enableVerticalLabelFineTuning(bool labelStatus)
{
    m_applyFineTuning = labelStatus;
}

void QGLchart::setManualHorzScaleRelPos(QVector2D manualHorzScalePos)
{
    m_manualHorzScalePosEnabled = true;
    m_manualHorzScalePos = manualHorzScalePos;
}

QVector2D QGLchart::getManualHorzScaleRelPos() const
{
    return m_manualHorzScalePos;
}

/****************************************************************************************/
/*						        ToolTip Related functions								*/
/****************************************************************************************/

void QGLchart::enableToolTip(bool state)
{
    m_ToolTipEnabled = state;
}

void QGLchart::setmaxAcceptableDistance(QVector2D distance)
{
    m_maxAcceptableDistance = distance;
}

bool QGLchart::isToolTipEnabled()
{
    return m_ToolTipEnabled;
}

QVector2D QGLchart::maxAcceptableDistance()
{
    return m_maxAcceptableDistance;
}

ChartType QGLchart::getChartType() const
{
    return m_chartType;
}

QVector2D QGLchart::infoOfNearestData(QPointF mousePosPoint, int inputDataIndex)
{
    QVector<QVector2D> dataHolder;
    QVector2D mousePos(mousePosPoint);
    QVector2D desiredData;

    int tempDataIndex = 0;

    if ((m_addedSeriesData.at(inputDataIndex).isVisible()) && (m_addedSeriesData.at(inputDataIndex).seriesSize() > 0))
    {
        dataHolder = m_addedSeriesData.at(inputDataIndex).pointsVector();

        for (int i = 1; i < dataHolder.size(); i++)
        {
            if (mousePos.distanceToPoint(dataHolder.at(tempDataIndex)) > mousePos.distanceToPoint(dataHolder.at(i)))
                tempDataIndex = i;
        }

    }

    m_currentMarkerIndex = tempDataIndex;

    if (!dataHolder.isEmpty())
    {
        if (!m_maxAcceptableDistance.isNull())
        {
            if ((abs(mousePos.x() - dataHolder.at(tempDataIndex).x()) < m_maxAcceptableDistance.x()) &&
                (abs(mousePos.y() - dataHolder.at(tempDataIndex).y()) < m_maxAcceptableDistance.y()))
            {
                desiredData = dataHolder.at(tempDataIndex);
            }
        }
        else
            desiredData = dataHolder.at(tempDataIndex);
    }

    if ((m_verticalAxes->scaleType() == ScaleType::Logrithmic) && (!desiredData.isNull()))
        desiredData.setY(pow(10, desiredData.y()));

    return desiredData;
}

/****************************************************************************************/
/*						           Zoom Related functions								*/
/****************************************************************************************/

void QGLchart::zoomEnable(bool status)
{
    m_zoomState = status;
    /*if (status == false)
    {
        m_zoomAreaSelected = false;
    }*/
}

void QGLchart::zoom(qreal factor)
{
    if (factor <= 0)
    {
        qWarning() << QObject::tr("Factor should be a posistive non zero number");
        return;
    }
    if (factor == 1)
        return;
    else if (factor > 1)
        zoomIn(1.0 * factor);
    else
        zoomOut(1.0 / factor);
}

void QGLchart::zoomIn(qreal factor)
{
    if (factor < 1)
    {
        qWarning() << QObject::tr("Factor should be a greater than or equal to one");
        return;
    }
    else if (factor == 1)
        return;
    else
    {
        QRectF rect = m_geometry;
        rect.setWidth(rect.width() / factor);
        rect.setHeight(rect.height() / factor);
        rect.moveCenter(m_geometry.center());
        zoomIn(rect);
    }
}

void QGLchart::zoomIn(const QRectF &rect)
{
    if (!rect.isValid())
        return;
    QRectF r = rect.normalized();
    //m_drawRegion = r;
    m_zoomState = true;
}

void QGLchart::zoomOut(qreal factor)
{
    if (factor < 1)
    {
        qWarning() << QObject::tr("Factor should be a greater than or equal to one");
        return;
    }
    else if (factor == 1)
        return;
    else
    {
        QRectF rect = m_geometry;
        rect.setWidth(rect.width() * factor);
        rect.setHeight(rect.height() * factor);
        rect.moveCenter(m_geometry.center());

        if (!rect.isValid())
            return;
        QRectF r = rect.normalized();
        //m_drawRegion = r;
        m_zoomState = true;
    }
}

void QGLchart::zoomReset()
{
    m_drawRegion = m_geometry;  // currently not in use, possibly wrong!
    m_zoomState = false;
}

bool QGLchart::isZoomed()
{
    return m_zoomState;
}

QVector4D QGLchart::wheelZoomDataRange(QPoint mousePos, QPoint angleDelta)
{
    QVector2D rangeX;
    QVector2D rangeY;
    QVector4D dataZoomRanges;
    qreal zoomStep = 0.1 * angleDelta.y();
    if (m_zoomAreaSelected)
    {
        rangeX.setX(m_zoomedData.x());
        rangeX.setY(m_zoomedData.y());
        rangeY.setX(m_zoomedData.z());
        rangeY.setY(m_zoomedData.w());
    }
    else
    {
        rangeX = m_ChartHorizantalRange;
        rangeY = m_ChartVerticalRange;
    }

    float horizantalZoomStep = (((float)mousePos.x() / m_drawRegion.width()) * (rangeX.y() - rangeX.x())) * zoomStep;
    float verticalZoomStep = (((float)mousePos.y() / m_drawRegion.height()) * (rangeY.y() - rangeY.x())) * zoomStep;

    dataZoomRanges.setX(rangeX.x() + horizantalZoomStep);																			// Left
    dataZoomRanges.setY(rangeX.y() - (horizantalZoomStep * ((m_drawRegion.width() - mousePos.x()) / (float)mousePos.x())));			//Right												// Right

    dataZoomRanges.setZ(rangeY.x() + (verticalZoomStep * ((m_drawRegion.height() - mousePos.y()) / (float)mousePos.y())));			// Bottom
    dataZoomRanges.setW(rangeY.y() - verticalZoomStep);																				// Top

    if (dataZoomRanges.x() < m_ChartHorizantalRange.x())
        dataZoomRanges.setX(m_ChartHorizantalRange.x());

    if (dataZoomRanges.y() > m_ChartHorizantalRange.y())
        dataZoomRanges.setY(m_ChartHorizantalRange.y());

    if (dataZoomRanges.z() < m_ChartVerticalRange.x())
        dataZoomRanges.setZ(m_ChartVerticalRange.x());

    if (dataZoomRanges.w() > m_ChartVerticalRange.y())
        dataZoomRanges.setW(m_ChartVerticalRange.y());

    return dataZoomRanges;
}

QVector4D QGLchart::dataInZoomArea()
{
    QVector2D rangeX;
    QVector2D rangeY;
    QVector4D dataZoomRanges;
    if (m_zoomAreaSelected)
    {
        rangeX.setX(m_zoomedData.x());
        rangeX.setY(m_zoomedData.y());
        rangeY.setX(m_zoomedData.z());
        rangeY.setY(m_zoomedData.w());

        dataZoomRanges.setX(((float)m_zoomArea.x() / m_drawRegion.width()) * (rangeX.y() - rangeX.x()));					// Left
        dataZoomRanges.setX(dataZoomRanges.x() + rangeX.x());

        dataZoomRanges.setY((((float)m_zoomArea.bottomRight().x() + 1) / m_drawRegion.width()) * (rangeX.y() - rangeX.x()));		// Right
        dataZoomRanges.setY(dataZoomRanges.y() + rangeX.x());

        dataZoomRanges.setZ((((float)m_zoomArea.bottomRight().y() + 1) / m_drawRegion.height()) * (rangeY.y() - rangeY.x()));		// Bottom
        dataZoomRanges.setZ(rangeY.y() - dataZoomRanges.z());

        dataZoomRanges.setW(((float)m_zoomArea.y() / m_drawRegion.height()) * (rangeY.y() - rangeY.x()));					// Top
        dataZoomRanges.setW(rangeY.y() - dataZoomRanges.w());
    }
    else
    {
        rangeX = m_ChartHorizantalRange;
        rangeY = m_ChartVerticalRange;

        dataZoomRanges.setX(((float)m_zoomArea.x() / m_drawRegion.width()) * (rangeX.y() - rangeX.x()));					// Left
        dataZoomRanges.setX(rangeX.x() + dataZoomRanges.x());
        dataZoomRanges.setY((((float)m_zoomArea.bottomRight().x() + 1) / m_drawRegion.width()) * (rangeX.y() - rangeX.x()));		// Right
        dataZoomRanges.setY(rangeX.x() + dataZoomRanges.y());
        dataZoomRanges.setZ((((float)m_zoomArea.bottomRight().y() + 1) / m_drawRegion.height()) * (rangeY.y() - rangeY.x()));		// Bottom
        dataZoomRanges.setZ(rangeY.y() - dataZoomRanges.z());
        dataZoomRanges.setW(((float)m_zoomArea.y() / m_drawRegion.height()) * (rangeY.y() - rangeY.x()));					// Top
        dataZoomRanges.setW(rangeY.y() - dataZoomRanges.w());
    }
    return dataZoomRanges;
}

void QGLchart::determineZoomArea(QPoint startPoint, QPoint endPoint, ZoomType zoomType)
{
    if (zoomType == None)		// Could have used switch instead
    {
        return;
    }
    else if (zoomType == HorizantalZoom)
    {
        if (startPoint.x() < endPoint.x())
        {
            m_zoomArea.setLeft(startPoint.x());
            m_zoomArea.setRight(endPoint.x());
        }
        else
        {
            m_zoomArea.setLeft(endPoint.x());
            m_zoomArea.setRight(startPoint.x());
        }
        m_zoomArea.setTop(0);
        m_zoomArea.setHeight(m_drawRegion.height());
    }
    else if (zoomType == VerticalZoom)
    {
        if (startPoint.y() < endPoint.y())
        {
            m_zoomArea.setTop(startPoint.y());
            m_zoomArea.setBottom(endPoint.y());
        }
        else
        {
            m_zoomArea.setTop(endPoint.y());
            m_zoomArea.setBottom(startPoint.y());
        }
        m_zoomArea.setLeft(0);
        m_zoomArea.setWidth(m_drawRegion.width());
    }
    else if (zoomType == RectangleZoom)
    {
        if (startPoint.x() < endPoint.x())
        {
            if (startPoint.y() < endPoint.y())
            {
                m_zoomArea.setTopLeft(startPoint);
                m_zoomArea.setBottomRight(endPoint);
            }
            else
            {
                m_zoomArea.setBottomLeft(startPoint);
                m_zoomArea.setTopRight(endPoint);
            }
        }
        else
        {
            if (startPoint.y() < endPoint.y())
            {
                m_zoomArea.setTopRight(startPoint);
                m_zoomArea.setBottomLeft(endPoint);
            }
            else
            {
                m_zoomArea.setBottomRight(startPoint);
                m_zoomArea.setTopLeft(endPoint);
            }
        }
    }
    else if (zoomType == SingleClickZoom)
    {
        // to be developed later
    }
}

void QGLchart::determineZoomAreaRectangle(QPoint startPoint, QPoint endPoint, ZoomType zoomType)
{
    if (zoomType == HorizantalZoom)
    {
        if (startPoint.x() < endPoint.x())
        {
            m_zoomAreaRectangle.setLeft(startPoint.x());
            m_zoomAreaRectangle.setRight(endPoint.x());
        }
        else
        {
            m_zoomAreaRectangle.setLeft(endPoint.x());
            m_zoomAreaRectangle.setRight(startPoint.x());
        }
        m_zoomAreaRectangle.setTop(startPoint.y() - 7);
        m_zoomAreaRectangle.setBottom(startPoint.y() + 7);
    }
    else if (zoomType == VerticalZoom)
    {
        if (startPoint.y() < endPoint.y())
        {
            m_zoomAreaRectangle.setTop(startPoint.y());
            m_zoomAreaRectangle.setBottom(endPoint.y());
        }
        else
        {
            m_zoomAreaRectangle.setTop(endPoint.y());
            m_zoomAreaRectangle.setBottom(startPoint.y());
        }
        m_zoomAreaRectangle.setLeft(startPoint.x() - 7);
        m_zoomAreaRectangle.setRight(startPoint.x() + 7);
    }
    else if (zoomType == RectangleZoom)
    {
        if (startPoint.x() < endPoint.x())
        {
            if (startPoint.y() < endPoint.y())
            {
                m_zoomAreaRectangle.setTopLeft(startPoint);
                m_zoomAreaRectangle.setBottomRight(endPoint);
            }
            else
            {
                m_zoomAreaRectangle.setBottomLeft(startPoint);
                m_zoomAreaRectangle.setTopRight(endPoint);
            }
        }
        else
        {
            if (startPoint.y() < endPoint.y())
            {
                m_zoomAreaRectangle.setTopRight(startPoint);
                m_zoomAreaRectangle.setBottomLeft(endPoint);
            }
            else
            {
                m_zoomAreaRectangle.setBottomRight(startPoint);
                m_zoomAreaRectangle.setTopLeft(endPoint);
            }
        }
    }
}

ZoomType QGLchart::determineZoomType(QPoint startPoint, QPoint endPoint)
{
    ZoomType zoomType;
    QPoint diff = endPoint - startPoint;
    diff.setX(abs(diff.x()));
    diff.setY(abs(diff.y()));
    if (diff.manhattanLength() < 5)
    {
        zoomType = SingleClickZoom;
    }
    else if (((diff.x() < 3) || (((qreal)diff.x() / diff.y()) < 0.2)) && (diff.x() < 15))
    {
        zoomType = VerticalZoom;
    }
    else if (((diff.y() < 3) || (((qreal)diff.y() / diff.x()) < 0.2)) && (diff.y() < 15))
    {
        zoomType = HorizantalZoom;
    }
    else
    {
        zoomType = RectangleZoom;
    }

    return zoomType;
}

QPoint QGLchart::projectPointToDrawRegion(QPoint endPoint)
{
    QPoint projectedPoint;
    if (m_drawRegion.contains(endPoint))
    {
        return endPoint;
    }

    if (endPoint.y() < m_drawRegion.top())
    {
        projectedPoint.setY(m_drawRegion.top());
    }
    else if (endPoint.y() < m_drawRegion.bottom())
    {
        projectedPoint.setY(endPoint.y());
    }
    else
    {
        projectedPoint.setY(m_drawRegion.bottom());
    }

    if (endPoint.x() < m_drawRegion.left())
    {
        projectedPoint.setX(m_drawRegion.left());
    }
    else if (endPoint.x() < m_drawRegion.right())
    {
        projectedPoint.setX(endPoint.x());
    }
    else
    {
        projectedPoint.setX(m_drawRegion.right());
    }

    return projectedPoint;
}

void QGLchart::setContentsMargins(const QMarginsF &margins)
{
    m_contentsMargin = margins;
}

QMarginsF QGLchart::contentsMargins() const
{
    return m_contentsMargin;
}

QVector2D QGLchart::getChartHorizantalRange()
{
    QVector2D horizantalRange;
    if ((m_horizantalAxes->getRange().second - m_horizantalAxes->getRange().first) == 0)
    {
        if (m_verticalChart)
        {
            horizantalRange.setX(minYvalueInAllSeries());
            horizantalRange.setY(maxYvalueInAllSeries());
        }
        else
        {
            horizantalRange.setX(minXvalueInAllSeries());
            horizantalRange.setY(maxXvalueInAllSeries());
        }
    }
    else
    {
        horizantalRange.setX(m_horizantalAxes->getRange().first);
        horizantalRange.setY(m_horizantalAxes->getRange().second);
    }
    /*horizantalRange.setX(horizantalRange.x() * 1.01);
    horizantalRange.setY(horizantalRange.y() * 1.01);*/

    /*float meanRangeValue = (horizantalRange.y() + horizantalRange.x()) / 2.0;
    if ((abs(horizantalRange.y() - horizantalRange.x()) / abs(meanRangeValue)) < 0.05)
    {
        horizantalRange.setX(meanRangeValue * 0.95);
        horizantalRange.setY(meanRangeValue * 1.05);
    }*/

    /*if (abs(horizantalRange.y() - horizantalRange.x()) < 0.5)
    {
        horizantalRange.setX(horizantalRange.x() - 0.25);
        horizantalRange.setY(horizantalRange.y() + 0.25);
    }*/
    if (m_chartType == ChartType::ScatterPlot)
    {
        //horizantalRange = setRangeMinValue(horizantalRange, m_horizantalAxes->scaleType());
    }

    /*if ((abs(horizantalRange.x() - m_previousHorizantalRange.x()) / abs(m_previousHorizantalRange.x())) > 0.01)
        m_previousHorizantalRange.setX(horizantalRange.x());
    if ((abs(horizantalRange.y() - m_previousHorizantalRange.y()) / abs(m_previousHorizantalRange.y())) > 0.01)
        m_previousHorizantalRange.setY(horizantalRange.y());
    return m_previousHorizantalRange;*/
    return horizantalRange;
}

QVector2D QGLchart::getChartVerticalRange()
{
    QVector2D verticalRange;
    QPair<bool, bool> partialRange = m_verticalAxes->isPartialRangeSet();
    if ((m_verticalAxes->getRange().second - m_verticalAxes->getRange().first) == 0)
    {
        if (m_verticalChart)
        {
            verticalRange.setX(minXvalueInAllSeries());
            verticalRange.setY(maxXvalueInAllSeries());

//            verticalRange.setX(0);
//            verticalRange.setY(10);
        }
        else
        {
            verticalRange.setX(minYvalueInAllSeries());
            verticalRange.setY(maxYvalueInAllSeries());

//            verticalRange.setX(0);
//            verticalRange.setY(10);
        }
    }
    else if ((partialRange.first) || (partialRange.second))
    {
        if (partialRange.first)
        {
            verticalRange.setX(m_verticalAxes->getRange().first);
            verticalRange.setY(maxYvalueInAllSeries());
        }
        else
        {
            verticalRange.setX(minYvalueInAllSeries());
            verticalRange.setY(m_verticalAxes->getRange().second);


//            verticalRange.setX(0);
//            verticalRange.setY(10);
        }

    }
    else
    {
        verticalRange.setX(m_verticalAxes->getRange().first);
        verticalRange.setY(m_verticalAxes->getRange().second);

//        verticalRange.setX(0);
//        verticalRange.setY(10);
    }

    verticalRange = setRangeMinValue(verticalRange, m_verticalAxes->scaleType());
    //return verticalRange;

    if (m_chartType == ChartType::ScatterPlot)
    {
        if ((m_verticalAxes->getRange().second - m_verticalAxes->getRange().first) == 0)
        {
            verticalRange.setX(verticalRange.x() - m_scatterLowerRangeMargin);
            verticalRange.setY(verticalRange.y() + m_scatterUpperRangeMargin);
        }
        return verticalRange + m_ChartVerticalRangeOffset;
    }

    else
    {
        fineTuneVerticalRange(verticalRange);
        return m_previousVerticalRange + m_ChartVerticalRangeOffset;
    }
}

void QGLchart::fineTuneVerticalRange(QVector2D verticalRange)
{
    if (m_previousVerticalRange.isNull())
    {
        m_previousVerticalRange.setX(verticalRange.x() - m_verticalLowerRangeMargin);
        m_previousVerticalRange.setY(verticalRange.y() + m_verticalUpperRangeMargin);
    }

    if ((m_previousVerticalRange.y() - verticalRange.y()) < 0)
        m_previousVerticalRange.setY(verticalRange.y() + m_verticalUpperRangeMargin);
    else if ((m_previousVerticalRange.y() - verticalRange.y()) < (m_verticalUpperRangeMargin / 2.0))
        m_previousVerticalRange.setY(verticalRange.y() + (m_verticalUpperRangeMargin / 2.0));
    else if ((m_previousVerticalRange.y() - verticalRange.y()) > (m_verticalUpperRangeMargin * 1.5))
        m_previousVerticalRange.setY(verticalRange.y() - (m_verticalUpperRangeMargin / 2.0));
    else if ((m_previousVerticalRange.y() - verticalRange.y()) > (m_verticalUpperRangeMargin * 2.0))
        m_previousVerticalRange.setY(verticalRange.y() - m_verticalUpperRangeMargin);


    if ((verticalRange.x() - m_previousVerticalRange.x()) < 0)
        m_previousVerticalRange.setX(verticalRange.x() - m_verticalLowerRangeMargin);
    else if ((verticalRange.x() - m_previousVerticalRange.x()) < (m_verticalLowerRangeMargin / 2.0))
        m_previousVerticalRange.setX(verticalRange.x() - (m_verticalLowerRangeMargin / 2.0));
    else if ((verticalRange.x() - m_previousVerticalRange.x()) > (m_verticalLowerRangeMargin * 1.5))
        m_previousVerticalRange.setX(verticalRange.x() + (m_verticalLowerRangeMargin / 2.0));
    else if ((verticalRange.x() - m_previousVerticalRange.x()) > (m_verticalLowerRangeMargin * 2.0))
        m_previousVerticalRange.setX(verticalRange.x() + m_verticalLowerRangeMargin);
}

void QGLchart::setVerticalUpperRangeMargin(double margin)
{
    m_verticalUpperRangeMargin = margin;
}

double QGLchart::getVerticalUpperRangeMargin()
{
    return m_verticalUpperRangeMargin;
}

void QGLchart::setVerticalLowerRangeMargin(double margin)
{
    m_verticalLowerRangeMargin = margin;
}

double QGLchart::getVerticalLowerRangeMargin()
{
    return m_verticalLowerRangeMargin;
}

void QGLchart::setScatterUpperRangeMargin(double margin)
{
    m_scatterUpperRangeMargin = margin;
}

void QGLchart::setScatterLowerRangeMargin(double margin)
{
    m_scatterLowerRangeMargin = margin;
}

double QGLchart::getScatterUpperRangeMargin()
{
    return m_scatterUpperRangeMargin;
}

double QGLchart::getScatterLowerRangeMargin()
{
    return m_scatterLowerRangeMargin;
}

QVector2D QGLchart::setRangeMinValue(QVector2D range, ScaleType scaleType)
{
    QVector2D newRange;

    int upperDigitNumber = floor(log10(abs(range.y()) + std::numeric_limits<double>::epsilon())) + 1;
    int lowerDigitNumber = floor(log10(abs(range.x()) + std::numeric_limits<double>::epsilon())) + 1;

    if (abs(upperDigitNumber - lowerDigitNumber) > 1)
        newRange = range;
    else
    {
        if (scaleType == ScaleType::Linear) {
            if (abs(range.y() - range.x()) < pow(10, upperDigitNumber - 3))
            {
                float rangeDiff = pow(10, upperDigitNumber - 3) - abs(range.y() - range.x());
                newRange.setX(range.x() - rangeDiff / 2.0);
                newRange.setY(range.y() + rangeDiff / 2.0);
            }
            else
                newRange = range;
        }
        else if (scaleType == ScaleType::Logrithmic)
        {
            if (abs(range.y() - range.x()) < 0.1)
            {
                float rangeDiff = 0.1 - abs(range.y() - range.x());
                newRange.setX(range.x() - rangeDiff / 2.0);
                newRange.setY(range.y() + rangeDiff / 2.0);
            }
            else
                newRange = range;
        }
    }
    return newRange;
}

float QGLchart::minXvalueInAllSeries()
{
    float m_minX = 1e100;
    for (int i = 0; i < m_addedSeriesData.count(); i++)
    {
        if (m_addedSeriesData.at(i).isVisible())
        {
            float minX = m_addedSeriesData[i].minX();
            if (m_minX > minX)
            {
                m_minX = minX;
            }
        }
    }
    return m_minX;
}

float QGLchart::maxXvalueInAllSeries()
{
    float m_maxX = -1e100;
    for (int i = 0; i < m_addedSeriesData.count(); i++)
    {
        if (m_addedSeriesData.at(i).isVisible())
        {
            float maxX = m_addedSeriesData[i].maxX();
            if (m_maxX < maxX)
            {
                m_maxX = maxX;
            }
        }
    }
    return m_maxX;
}

float QGLchart::minYvalueInAllSeries()
{
    float m_minY = 1e100;
    for (int i = 0; i < m_addedSeriesData.count(); i++)
    {
        if (m_addedSeriesData.at(i).isVisible())
        {
            float minY = m_addedSeriesData[i].minY();
            if (m_minY > minY)
            {
                m_minY = minY;
            }
        }
    }
    return m_minY;
}

float QGLchart::maxYvalueInAllSeries()
{
    float m_maxY = -1e100;
    for (int i = 0; i < m_addedSeriesData.count(); i++)
    {
        if (m_addedSeriesData.at(i).isVisible())
        {
            float maxY = m_addedSeriesData[i].maxY();
            if (m_maxY < maxY)
            {
                m_maxY = maxY;
            }
        }
    }
    return m_maxY;
}

void QGLchart::enableSyncedZoom(bool syncedZoom)
{
    m_SyncedZoom = syncedZoom;
}

bool QGLchart::syncedZoom()
{
    return m_SyncedZoom;
}

void QGLchart::setZoomType(ZoomType zoomType)
{
    m_zoomType = zoomType;
    m_manualZoomType = true;
}

void QGLchart::disableManualZoom()
{
    m_manualZoomType = false;
}

bool QGLchart::isManualZoomActive()
{
    return m_manualZoomType;
}

ZoomType QGLchart::getZoomType()
{
    return m_zoomType;
}

////////////////// Area Selection Related functions /////////////////

void QGLchart::areaSelectionEnable(bool status)
{
    m_areaSelectionState = status;
}

/*----------------------- Spectrum functions ----------------------*/

void QGLchart::initializeGL()
{
    makeObject();
    makeCurrent();
    f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();
    f->initializeOpenGLFunctions();

    // auto t1 = QOpenGLContext::currentContext()->functions()->glGetString(GL_VERSION);

    fExt = QOpenGLContext::currentContext()->extraFunctions();

    connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);
    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

    m_debugLogger = new QOpenGLDebugLogger(this);
    if (m_debugLogger->initialize())
    {
        qDebug() << "GL_DEBUG Debug Logger" << m_debugLogger << "\n";
        //connect(m_debugLogger, SIGNAL(messageLogged(QOpenGLDebugMessage)), this, SLOT(messageLogged(QOpenGLDebugMessage)));
        //m_debugLogger->startLogging(QOpenGLDebugLogger::SynchronousLogging);
    }
    else
    {
        qDebug() << "GL_DEBUG Debug Logger (NONE)\n";
    }

    initializeBoxProgram();
    initializeGridProgram();
    initializeZoomAreaProgram();
    initializeThresholdProgram();
    initializeMaskingProgram();
    if (m_chartType == ScatterPlot)
    {
        initializeScatterPlotProgram();
    }
    else
    {
        initializeSpectrumProgram();
    }
    initializeTextProgram();

    /*---waterfall related---*/
    QOpenGLFramebufferObjectFormat format;
    format.setMipmap(true);
    format.setTextureTarget(GL_TEXTURE_2D);
    format.setSamples(0);
    m_newWaterfallFrameBuffer = new QOpenGLFramebufferObject(1024, 1024, format);
    m_oldWaterfallFrameBuffer = new QOpenGLFramebufferObject(1024, 1024, format);

    f->glGenFramebuffers(1, &framebuffer);
    f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    m_newWaterfallFrameBuffer->bind();
    f->glClearColor(0.0, 0, 0.0, 0.0);
    f->glClear(GL_COLOR_BUFFER_BIT);
    f->glClearColor(0.0, 0.0, 0.0, 1.0);
    m_newWaterfallFrameBuffer->release();
    f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);

    m_oldWaterfallFrameBuffer->bind();
    f->glClearColor(0.0, 0.0, 0.0, 0.0);
    f->glClear(GL_COLOR_BUFFER_BIT);
    f->glClearColor(0.0, 0.0, 0.0, 1.0);
    m_oldWaterfallFrameBuffer->release();
    f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);

    initializeBoxWaterFallProgram();
    initializeColorPalettesProgram();
    initializeMarkerProgram();

    doneCurrent();
}

void QGLchart::paintGL()
{
    /*QTime *runTime = new QTime();
    runTime->start();*/

    makeCurrent();
    f->glClearColor(0.0, 0.0, 0.0, 0.0);
    f->glClear(GL_COLOR_BUFFER_BIT);
    f->glClearColor(0.0, 0.0, 0.0, 1.0);
    updateBoxProgram();

    if (m_chartType == Spectrum)
    {
        if (!m_currentChartAxis.isEmpty())
        {
            updateGridProgram();
        }

        for (int seriesIndex = 0; seriesIndex < m_addedSeriesData.count(); seriesIndex++)
        {
            if ((m_addedSeriesData.at(seriesIndex).isVisible()) && (m_addedSeriesData.at(seriesIndex).seriesSize() > 0))
            {
                updateSpectrumProgram(&m_addedSeriesData[seriesIndex]);
            }
        }
    }
    else if (m_chartType == ScatterPlot)
    {
        for (int seriesIndex = 0; seriesIndex < m_addedSeriesData.count(); seriesIndex++)
        {
            if ((m_addedSeriesData.at(seriesIndex).isVisible()) && (m_addedSeriesData.at(seriesIndex).seriesSize() > 0))
            {
                updateScatterPlotProgram(&m_addedSeriesData[seriesIndex]);
            }
        }

        if (!m_currentChartAxis.isEmpty())
        {
            updateGridProgram();
        }
    }
    else if (m_chartType == Waterfall)
    {
        if (changeWaterfall)
        {
            m_newWaterfallFrameBuffer->bind();
            f->glClearColor(0.0, 0.0, 0.0, 1.0);
            f->glClear(GL_COLOR_BUFFER_BIT);
            m_newWaterfallFrameBuffer->release();
            //f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);
            m_newWaterfallFrameBuffer->bind();
            updateOldBoxWaterFallProgram();
            m_newWaterfallFrameBuffer->release();
            //f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);
            m_newWaterfallFrameBuffer->bind();
            for (int seriesIndex = 0; seriesIndex < m_addedSeriesData.count(); seriesIndex++)
            {
                if (m_addedSeriesData.at(seriesIndex).seriesSize() > 0)
                {
                    updateSpectrumColorizedProgram(&m_addedSeriesData[seriesIndex]);
                    break;
                }
            }
            m_newWaterfallFrameBuffer->release();
            //f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);
            QOpenGLFramebufferObject* tmp;
            tmp = m_oldWaterfallFrameBuffer;
            m_oldWaterfallFrameBuffer = m_newWaterfallFrameBuffer;
            m_newWaterfallFrameBuffer = tmp;
            changeWaterfall = false;

            /*
            if (numberOfLine == 148)
            {
                numberOfLine = 0;

                int  elapsed = timeWaterfall.elapsed();
                //qDebug() << "numberOfLine = " << numberOfLine << " , elapsed = " << elapsed;
                m_waterfallTimeVector.clear();
                for (int i = 0; i < 8; i++)
                {
                    m_waterfallTimeVector.append(elapsed - (i*elapsed / (8)));
                }
                m_waterfallTimeVector.append(0);
                timeWaterfall.start();
            }
            else
            {
                numberOfLine++;
            }
            */

        }

        //drawMarkerInformation();
//        f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);
        updateBoxWaterFallProgram();
//        f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);
//        updateColorPalettesProgram();
        updateGridProgram();
    }
    else if (m_chartType == HorizantalWaterfall)
    {
        if (changeHorizantalWaterfall)
        {
            m_newWaterfallFrameBuffer->bind();
            f->glClearColor(0.0, 0.0, 0.0, 1.0);
            f->glClear(GL_COLOR_BUFFER_BIT);
            m_newWaterfallFrameBuffer->release();
            f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);
            m_newWaterfallFrameBuffer->bind();
            updateOldBoxWaterFallProgram();
            m_newWaterfallFrameBuffer->release();
            f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);
            m_newWaterfallFrameBuffer->bind();
            updateSpectrumColorizedProgram(&m_addedSeriesData[0]);		// temporary
            m_newWaterfallFrameBuffer->release();
            f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);
            QOpenGLFramebufferObject* tmp;
            tmp = m_oldWaterfallFrameBuffer;
            m_oldWaterfallFrameBuffer = m_newWaterfallFrameBuffer;
            m_newWaterfallFrameBuffer = tmp;
            changeWaterfall = false;

            if (numberOfLine == 148)
            {
                numberOfLine = 0;

                int  elapsed = timeWaterfall.elapsed();
                //qDebug() << "numberOfLine = " << numberOfLine << " , elapsed = " << elapsed;
                m_waterfallTimeVector.clear();
                for (int i = 0; i < 8; i++)
                {
                    m_waterfallTimeVector.append(elapsed - (i*elapsed / (8)));
                }
                m_waterfallTimeVector.append(0);
                timeWaterfall.start();
            }
            else
            {
                numberOfLine++;
            }

        }

        //drawMarkerInformation();
        f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);
        updateBoxWaterFallProgram();
        f->glBindFramebuffer(GL_FRAMEBUFFER, framebuffer + 1);
        updateColorPalettesProgram();
        updateGridProgram();
    }

    if (!m_addedSeriesData.empty())
    {
        drawHorizontalNumbers();
        if(m_verticalAxes->visibility())
            drawVerticalNumbers();
    }

    if (((m_zoomState == true) && (m_mouseMoved == true)) || ((m_areaSelectionState == true) && (m_mouseMoved == true)))
    {
        updateZoomAreaProgram(m_zoomType);
    }

    if (!m_titleText.isEmpty())
    {
        drawChartTitle();
    }

    if (m_verticalAxes->visibility()&&(!m_verticalAxes->titleText().isEmpty()) && (m_chartType != Waterfall) && (!m_addedSeriesData.empty()))
    {
        drawVerticalLabel();
    }

    if (m_verticalAxes->visibility() && (!m_verticalAxes->labelsText().isEmpty()) && (m_chartType != Waterfall) && (!m_addedSeriesData.empty()))
    {
        drawVerticalScaleLabel();
    }

    if ((!m_horizantalAxes->titleText().isEmpty()) && (!m_addedSeriesData.empty()))
    {
        drawHorizontalLabel();
    }

    if ((!m_horizantalAxes->labelsText().isEmpty()) && (!m_addedSeriesData.empty()))
    {
        drawHorizontalScaleLabel();
    }

    if (yAxis2->visibility())
    {
        drawYaxis2Numbers();
        if (!yAxis2->titleText().isEmpty())
            drawYaxis2Text();
        if (!yAxis2->labelsText().isEmpty())
            drawYaxis2LabelText();
    }

    if (m_thresholdEnabled)
    {
        if (m_thresholdPoints.isEmpty())
            initializeThresholdPoints(m_thresholdType);
        updateThresholdProgram();
    }

    if ((m_maskingEnabled) && (m_maskingCenterPoints.size() > 0))
    {
        updateMaskingProgram();
    }

    if ((m_markerEnabled) && (!m_markerCenterPoint.isEmpty()))
        updateMarkerProgram();

    //QString framesPerSecond;
    //if (const int elapsed = time.elapsed()) {

    //	framesPerSecond.setNum(m_frame / (elapsed / 1e3), 'f', 2);
    //	RenderText(framesPerSecond.toStdString(), 0.0f, 15.0f, 0.5f, QVector3D(1.0, 1.0f, 0.2f));
    //}
    //if (!(m_frame % 100)) {
    //	time.start();
    //	m_frame = 0;
    //}
    //++m_frame;

    //int elapsed = runTime->elapsed();
    //RenderText(QString::number(elapsed).toStdString(), 50.0f, 15.0f, 0.5f, QVector3D(1.0, 1.0f, 0.2f));
}

void QGLchart::messageLogged(const QOpenGLDebugMessage &msg)
{
    QString error;

    // Format based on severity
    switch (msg.severity())
    {
    case QOpenGLDebugMessage::NotificationSeverity:
        error += "--";
        break;
    case QOpenGLDebugMessage::HighSeverity:
        error += "!!";
        break;
    case QOpenGLDebugMessage::MediumSeverity:
        error += "!~";
        break;
    case QOpenGLDebugMessage::LowSeverity:
        error += "~~";
        break;
    }

    error += " (";

    // Format based on source
#define CASE(c) case QOpenGLDebugMessage::c: error += #c; break
    switch (msg.source())
    {
        CASE(APISource);
        CASE(WindowSystemSource);
        CASE(ShaderCompilerSource);
        CASE(ThirdPartySource);
        CASE(ApplicationSource);
        CASE(OtherSource);
        CASE(InvalidSource);
    }
#undef CASE

    error += " : ";

    // Format based on type
#define CASE(c) case QOpenGLDebugMessage::c: error += #c; break
    switch (msg.type())
    {
        CASE(ErrorType);
        CASE(DeprecatedBehaviorType);
        CASE(UndefinedBehaviorType);
        CASE(PortabilityType);
        CASE(PerformanceType);
        CASE(OtherType);
        CASE(MarkerType);
        CASE(GroupPushType);
        CASE(GroupPopType);
    }
#undef CASE

    error += ")";
    qDebug() << qPrintable(error) << "\n" << qPrintable(msg.message()) << "\n";
}

void QGLchart::resizeGL(int w, int h)
{
    m_geometry.setWidth(w);
    m_geometry.setHeight(h);
}

void QGLchart::teardownGL()
{
    makeCurrent();
    m_textBuffer.destroy();
    m_textObject.destroy();
    delete m_textProgram;
    Characters.clear();

    m_boxBuffer.destroy();
    m_boxObject.destroy();
    delete m_boxProgram;

    m_horizontalGridBuffer.destroy();
    m_horizontalGridObject.destroy();
    delete m_horizontalGridProgram;

    m_verticalGridBuffer.destroy();
    m_verticalGridObject.destroy();
    delete m_verticalGridProgram;

    m_spectrumBuffer.destroy();
    m_spectrumObject.destroy();
    delete m_spectrumProgram;

    delete m_newWaterfallFrameBuffer;
    delete m_oldWaterfallFrameBuffer;
    //f->glDeleteFramebuffers(1, &framebuffer);

    /*m_markerBuffer.destroy();
    m_markerObject.destroy();
    delete m_markerProgram;

    m_deltaMarkerBuffer.destroy();
    m_deltaMarkerObject.destroy();
    delete m_deltaMarkerProgram;

    m_horizontalCursorBuffer.destroy();
    m_horizontalCursorObject.destroy();
    delete m_horizontalCursorProgram;


    m_verticalCursorBuffer.destroy();
    m_verticalCursorObject.destroy();
    delete m_verticalCursorProgram;


    m_zoomAreaBuffer.destroy();
    m_zoomAreaObject.destroy();
    delete m_zoomAreaProgram;


    m_boxMaskPositionBuffer.destroy();
    m_boxMaskCenterBuffer.destroy();
    m_boxMaskObject.destroy();
    delete m_boxMaskProgram;*/
    doneCurrent();
}

void QGLchart::makeObject()
{
    m_drawRegion.setTopLeft(QPoint(-1, 1));
    m_drawRegion.setBottomRight(QPoint(1, -1));

    if (m_contentsMargin.isNull())
    {
        m_contentsMargin.setLeft(0.03);
        m_contentsMargin.setBottom(0.1);
        m_contentsMargin.setRight(0);
        m_contentsMargin.setTop(0);
    }

    m_xRange.first = 0;
    m_xRange.second = 20;

    if (m_currentChartAxis.isEmpty())
    {
        // output some sort of error
    }
    else
    {
        m_verticalAxes = axes(Qt::AlignLeft).at(0);
        m_horizantalAxes = axes(Qt::AlignBottom).at(0);
    }

    for (int i = 0; i < 17; i++)
    {
        m_verticalGridPosition.append(QVector2D(i, 0.0f));
        m_verticalGridPosition.append(QVector2D(i, 16.0f));

        m_horizontalGridPosition.append(QVector2D(0.0f, i));
        m_horizontalGridPosition.append(QVector2D(16.0f, i));
    }

    /*--- Waterfall related variables ---*/
    for (int i = 0; i < 9; i++)
    {
        m_colorPalettesPosition.append(QVector2D(-1.0f, i * (1.0f / 8.0f)));
        m_colorPalettesPosition.append(QVector2D(1.0f, i * (1.0f / 8.0f)));
    }
    m_boxWaterFallPosition.append(QVector4D(-1.0f, 1.0f, 0.0f, 1.0f));
    m_boxWaterFallPosition.append(QVector4D(-1.0f, -1.0f, 0.0f, 0.0f));
    m_boxWaterFallPosition.append(QVector4D(1.0f, -1.0f, 1.0f, 0.0f));
    m_boxWaterFallPosition.append(QVector4D(1.0f, -1.0f, 1.0f, 0.0f));
    m_boxWaterFallPosition.append(QVector4D(-1.0f, 1.0f, 0.0f, 1.0f));
    m_boxWaterFallPosition.append(QVector4D(1.0f, 1.0f, 1.0f, 1.0f));

    m_boxMaskPosition.append(QVector2D(-1.0f, -1.0f));
    m_boxMaskPosition.append(QVector2D(1.0f, -1.0f));
    m_boxMaskPosition.append(QVector2D(-1.0f, 1.0f));
    m_boxMaskPosition.append(QVector2D(1.0f, 1.0f));

    m_markerPosition.append(QVector2D(-1.0f, -1.0f));
    m_markerPosition.append(QVector2D(1.0f, -1.0f));
    m_markerPosition.append(QVector2D(-1.0f, 1.0f));
    m_markerPosition.append(QVector2D(1.0f, 1.0f));

    m_scatterBoxPosition.append(QVector2D(-0.5f, -0.5f));
    m_scatterBoxPosition.append(QVector2D(0.5f, -0.5f));
    m_scatterBoxPosition.append(QVector2D(-0.5f, 0.5f));
    m_scatterBoxPosition.append(QVector2D(0.5f, 0.5f));

    m_boxPosition.append(QVector3D(-1.0f, -1.0f, 0.0f));
    m_boxPosition.append(QVector3D(1.0f, -1.0f, 0.0f));
    m_boxPosition.append(QVector3D(-1.0f, 1.0f, 0.0f));
    m_boxPosition.append(QVector3D(1.0f, 1.0f, 0.0f));

}

void QGLchart::initializeBoxProgram()
{
    m_boxProgram = new QOpenGLShaderProgram();
    bool ty = m_boxProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/box.vert");
    m_boxProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/box.frag");
    m_boxProgram->link();
    m_boxProgram->bind();

    // Cache Uniform Locations
    u_boxModelToWorld = m_boxProgram->uniformLocation("modelToWorld");
    u_boxWorldToCamera = m_boxProgram->uniformLocation("worldToCamera");
    u_boxCameraToView = m_boxProgram->uniformLocation("cameraToView");
    u_boxColor = m_boxProgram->uniformLocation("boxColor");

    m_boxObject.create();
    m_boxObject.bind();

    m_boxBuffer.create();
    m_boxBuffer.bind();
    m_boxBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    rectToVector(m_drawRegion, &m_boxPosition);
    m_boxBuffer.allocate(m_boxPosition.constData(), m_boxPosition.count() * sizeof(GLfloat) * 3);

    m_boxProgram->enableAttributeArray(0);
    m_boxBuffer.bind();
    m_boxProgram->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    // Release (unbind) all
    m_boxBuffer.release();
    m_boxObject.release();

    m_boxProgram->release();
}

void QGLchart::updateBoxProgram()
{
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glBlendEquation(GL_FUNC_ADD);
    calculateViewPort(m_geometry, m_contentsMargin);
    f->glViewport(m_viewPortBottomLeft.x(), m_viewPortBottomLeft.y(), m_viewPortSize.first, m_viewPortSize.second);
    m_projection.setToIdentity();

    m_projection.ortho(-1.0f, 1.0f, -1.0f, 1.0f, -20.0f, 20.0f);

    m_boxProgram->bind();
    m_boxObject.bind();

    m_boxProgram->setUniformValue(u_boxWorldToCamera, m_camera.toMatrix());
    m_boxProgram->setUniformValue(u_boxCameraToView, m_projection);
    m_boxProgram->setUniformValue(u_boxModelToWorld, m_transform.toMatrix());
    m_boxProgram->setUniformValue(u_boxColor, m_background.Brush.color());

    f->glDrawArrays(GL_TRIANGLE_STRIP, 0, m_boxPosition.count());

    m_boxObject.release();
    m_boxProgram->release();
}

void QGLchart::initializeGridProgram()
{
    if (!m_currentChartAxis.isEmpty())
    {
        QList<QCustomAxis*> verticalGridAxes = axes(Qt::AlignLeft);
        QList<QCustomAxis*> horizantalGridAxes = axes(Qt::AlignBottom);

        if (!verticalGridAxes.isEmpty())
        {
            //Create Shader (DO not release unit VAO is created)
            m_verticalGridProgram = new QOpenGLShaderProgram();
            m_verticalGridProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/gridLine.vert");
            m_verticalGridProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/gridLine.frag");
            m_verticalGridProgram->link();
            m_verticalGridProgram->bind();

            // Cache Uniform Locations
            u_verticalGridModelToWorld = m_verticalGridProgram->uniformLocation("modelToWorld");
            u_verticalGridWorldToCamera = m_verticalGridProgram->uniformLocation("worldToCamera");
            u_verticalGridCameraToView = m_verticalGridProgram->uniformLocation("cameraToView");
            u_verticalGridColor = m_verticalGridProgram->uniformLocation("color");
            //Create Buffer (Do not release until VAO is created)


            //Create Vertex Array Object
            m_verticalGridObject.create();
            m_verticalGridObject.bind();

            m_verticalGridBuffer.create();
            m_verticalGridBuffer.bind();
            m_verticalGridBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
            m_verticalGridBuffer.allocate(m_verticalGridPosition.constData(), m_verticalGridPosition.count() * sizeof(GLfloat) * 2);



            m_verticalGridProgram->enableAttributeArray(0);
            m_verticalGridBuffer.bind();
            m_verticalGridProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);


            // Release (unbind) all
            m_verticalGridBuffer.release();
            m_verticalGridObject.release();
            m_verticalGridProgram->release();
        }
        if (!horizantalGridAxes.isEmpty())
        {
            //Create Shader (DO not release unit VAO is created)
            m_horizontalGridProgram = new QOpenGLShaderProgram();
            m_horizontalGridProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/gridLine.vert");
            m_horizontalGridProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/gridLine.frag");
            m_horizontalGridProgram->link();
            m_horizontalGridProgram->bind();

            // Cache Uniform Locations
            u_horizontalGridModelToWorld = m_horizontalGridProgram->uniformLocation("modelToWorld");
            u_horizontalGridWorldToCamera = m_horizontalGridProgram->uniformLocation("worldToCamera");
            u_horizontalGridCameraToView = m_horizontalGridProgram->uniformLocation("cameraToView");
            u_horizontalGridColor = m_horizontalGridProgram->uniformLocation("color");

            //Create Buffer (Do not release until VAO is created)


            //Create Vertex Array Object
            m_horizontalGridObject.create();
            m_horizontalGridObject.bind();

            m_horizontalGridBuffer.create();
            m_horizontalGridBuffer.bind();
            m_horizontalGridBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
            m_horizontalGridBuffer.allocate(m_horizontalGridPosition.constData(), m_horizontalGridPosition.count() * sizeof(GLfloat) * 2);



            m_horizontalGridProgram->enableAttributeArray(0);
            m_horizontalGridBuffer.bind();
            m_horizontalGridProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);


            // Release (unbind) all
            m_horizontalGridBuffer.release();
            m_horizontalGridObject.release();
            m_horizontalGridProgram->release();
        }
    }
}

void QGLchart::updateGridProgram()
{
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glBlendEquation(GL_FUNC_ADD);
    //calculateViewPort(m_geometry, m_contentsMargin);
    f->glViewport(m_viewPortBottomLeft.x() - 1, m_viewPortBottomLeft.y() - 1, m_viewPortSize.first + 2, m_viewPortSize.second + 2);
    m_projection.setToIdentity();
    if ((m_chartType == Waterfall) || (m_chartType == HorizantalWaterfall) || (m_chartType == ScatterPlot))
    {
        m_projection.ortho(0.0f, 1.0f, 0.0f /** 2.0f*/, 1.0f/* * 2.0f*/, -20.0f, 20.0f);	// The range results in ommision of grid lines

        m_horizontalGridProgram->bind();
        m_horizontalGridObject.bind();
        m_horizontalGridProgram->setUniformValue(u_horizontalGridWorldToCamera, m_camera.toMatrix());
        m_horizontalGridProgram->setUniformValue(u_horizontalGridCameraToView, m_projection);
        m_horizontalGridProgram->setUniformValue(u_horizontalGridModelToWorld, m_transform.toMatrix());
        m_horizontalGridProgram->setUniformValue(u_horizontalGridColor, m_horizantalAxes->gridLineColor());
        f->glLineWidth(2);
        f->glDrawArrays(GL_LINES, 0, m_horizontalGridPosition.count());
        m_horizontalGridObject.release();
        m_horizontalGridProgram->release();

        m_verticalGridProgram->bind();
        m_verticalGridObject.bind();
        m_verticalGridProgram->setUniformValue(u_verticalGridWorldToCamera, m_camera.toMatrix());
        m_verticalGridProgram->setUniformValue(u_verticalGridCameraToView, m_projection);
        m_verticalGridProgram->setUniformValue(u_verticalGridModelToWorld, m_transform.toMatrix());
        m_verticalGridProgram->setUniformValue(u_verticalGridColor, m_verticalAxes->gridLineColor());
        f->glDrawArrays(GL_LINES, 0, m_verticalGridPosition.count());
        m_verticalGridObject.release();
        m_verticalGridProgram->release();
    }
    else
    {
        if ((m_horizantalAxes->gridLineVisible()) && (m_verticalAxes->gridLineVisible()))
        {
            m_projection.ortho(-0.0005f, (float)m_horizantalAxes->gridLineCount(), -0.0005f, (float)m_verticalAxes->gridLineCount(), -20.0f, 20.0f);
        }
        else if ((m_horizantalAxes->gridLineVisible()) && (!m_verticalAxes->gridLineVisible()))
        {
            m_projection.ortho(-0.0005f, (float)m_horizantalAxes->gridLineCount(), -0.0005f, 1.0f, -20.0f, 20.0f);
        }
        else if ((!m_horizantalAxes->gridLineVisible()) && (m_verticalAxes->gridLineVisible()))
        {
            m_projection.ortho(-0.0005f, 1.0f, -0.0005f, (float)m_verticalAxes->gridLineCount(), -20.0f, 20.0f);
        }
        else if ((!m_horizantalAxes->gridLineVisible()) && (!m_verticalAxes->gridLineVisible()))
        {
            m_projection.ortho(-0.0005f, 1.0f, -0.0005f, 1.0f, -20.0f, 20.0f);
        }

        /*--- Horizantal layout update ---*/

        m_horizontalGridProgram->bind();
        m_horizontalGridObject.bind();

        m_horizontalGridProgram->setUniformValue(u_horizontalGridWorldToCamera, m_camera.toMatrix());
        m_horizontalGridProgram->setUniformValue(u_horizontalGridCameraToView, m_projection);
        m_horizontalGridProgram->setUniformValue(u_horizontalGridModelToWorld, m_transform.toMatrix());
        m_horizontalGridProgram->setUniformValue(u_horizontalGridColor, m_horizantalAxes->gridLineColor());
        f->glLineWidth(1);
        f->glDrawArrays(GL_LINES, 0, m_horizontalGridPosition.count());

        m_horizontalGridObject.release();
        m_horizontalGridProgram->release();


        /*--- Horizantal layout update ---*/

        m_verticalGridProgram->bind();
        m_verticalGridObject.bind();

        m_verticalGridProgram->setUniformValue(u_verticalGridWorldToCamera, m_camera.toMatrix());
        m_verticalGridProgram->setUniformValue(u_verticalGridCameraToView, m_projection);
        m_verticalGridProgram->setUniformValue(u_verticalGridModelToWorld, m_transform.toMatrix());
        m_verticalGridProgram->setUniformValue(u_verticalGridColor, m_verticalAxes->gridLineColor());
        f->glDrawArrays(GL_LINES, 0, m_verticalGridPosition.count());

        m_verticalGridObject.release();
        m_verticalGridProgram->release();
    }

    f->glDisable(GL_BLEND);

}

void QGLchart::initializeSpectrumProgram()
{
    //Create Shader (DO not release unit VAO is created)
    m_spectrumProgram = new QOpenGLShaderProgram();
    if (m_chartType == Spectrum)
    {
        m_spectrumProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/drawLine.vert");
        //m_spectrumProgram->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/resources/shaders/drawLine.geo");
        m_spectrumProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/drawLine.frag");
    }
    else if (m_chartType == Waterfall)
    {
        m_spectrumProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/drawLineColorized.vert");
        m_spectrumProgram->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/resources/shaders/drawLineColorized.geo");
        m_spectrumProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/drawLineColorized.frag");
    }
    else if (m_chartType == HorizantalWaterfall)
    {
        m_spectrumProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/drawVerticalLineColorized.vert");
        m_spectrumProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/drawVerticalLineColorized.frag");
    }
    m_spectrumProgram->link();
    m_spectrumProgram->bind();

    // Cache Uniform Locations
    u_spectrumModelToWorld = m_spectrumProgram->uniformLocation("modelToWorld");
    u_spectrumWorldToCamera = m_spectrumProgram->uniformLocation("worldToCamera");
    u_spectrumCameraToView = m_spectrumProgram->uniformLocation("cameraToView");
    u_spectrumCameraToView2 = m_spectrumProgram->uniformLocation("secondCamera");
    u_spectrumColor = m_spectrumProgram->uniformLocation("color");
    u_spectrumTime = m_spectrumProgram->uniformLocation("Time");

    u_spectrumLineWidth = m_spectrumProgram->uniformLocation("lineWidth");
    u_spectrumMiterLimit = m_spectrumProgram->uniformLocation("miterLimit");
    u_spectrumWindowScale = m_spectrumProgram->uniformLocation("windowScale");

    //Create Vertex Array Object
    m_spectrumObject.create();
    m_spectrumObject.bind();

    m_spectrumBuffer.create();
    m_spectrumBuffer.bind();
    m_spectrumBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    int maxSeriesSizeHolder = maxSeriesSize();
    m_spectrumBuffer.allocate(NULL, maxSeriesSizeHolder * sizeof(GLfloat) * 2);

    //m_spectrumBuffer.bind();
    m_spectrumProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);
    m_spectrumProgram->enableAttributeArray(0);

    // Release (unbind) all
    m_spectrumBuffer.release();
    m_spectrumObject.release();
    m_spectrumProgram->release();
}

void QGLchart::updateSpectrumProgram(QCustomSeries* series)
{
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glBlendEquation(GL_FUNC_ADD);
    //calculateViewPort(m_geometry, m_contentsMargin);
    f->glViewport(m_viewPortBottomLeft.x(), m_viewPortBottomLeft.y(), m_viewPortSize.first, m_viewPortSize.second);
    m_projection.setToIdentity();

    if (m_zoomAreaSelected)
    {
        m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
    }
    else
    {
        m_ChartHorizantalRange = getChartHorizantalRange();
        m_ChartVerticalRange = getChartVerticalRange();

        QVector2D rangeX = m_ChartHorizantalRange;
        QVector2D rangeY = m_ChartVerticalRange;

        m_zoomedData.setX(rangeX.x());
        m_zoomedData.setY(rangeX.y());
        m_zoomedData.setZ(rangeY.x());
        m_zoomedData.setW(rangeY.y());

        m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
        //m_projection.ortho(rangeX.x(), rangeX.y(), rangeY.x(), rangeY.y(), -20.0f, 20.0f);
    }

    m_spectrumProgram->bind();
    m_spectrumObject.bind();

    m_spectrumBuffer.bind();
    m_spectrumProgram->setUniformValue(u_spectrumWorldToCamera, m_camera.toMatrix());
    m_spectrumProgram->setUniformValue(u_spectrumCameraToView, m_projection);
    m_spectrumProgram->setUniformValue(u_spectrumModelToWorld, m_transform.toMatrix());
    m_spectrumProgram->setUniformValue(u_spectrumColor, series->seriesColor());

    m_spectrumProgram->setUniformValue(u_spectrumLineWidth, 3.0f);
    m_spectrumProgram->setUniformValue(u_spectrumMiterLimit, -1.0f);
    m_spectrumProgram->setUniformValue(u_spectrumWindowScale, QVector2D((float)m_viewPortSize.first, (float)m_viewPortSize.second));

    //if (m_newSpectrumDataReceived)
    {
        QVector<QVector2D> inputVector(series->pointsVector());
        inputVector.prepend((2 * series->pointsVector().at(0)) - series->pointsVector().at(1));
        inputVector.append((2 * series->pointsVector().at(series->pointsVector().size() - 1)) - series->pointsVector().at(series->pointsVector().size() - 2));
        m_spectrumBuffer.allocate(inputVector.constData(), inputVector.count() * sizeof(GLfloat) * 2);
        m_newSpectrumDataReceived = false;
    }


    m_spectrumProgram->enableAttributeArray(0);
    m_spectrumBuffer.bind();
    m_spectrumProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);
    f->glEnable(GL_LINE_SMOOTH);
    f->glLineWidth(2);
    f->glDrawArrays(GL_LINE_STRIP_ADJACENCY, 0, series->seriesSize() + 2);

    m_spectrumObject.release();
    m_spectrumBuffer.release();
    m_spectrumProgram->release();
    f->glDisable(GL_BLEND);
    f->glViewport(0, 0, m_geometry.width(), m_geometry.height());
}

void QGLchart::updateSpectrumColorizedProgram(QCustomSeries* series)
{
    f->glViewport(0, 0, m_newWaterfallFrameBuffer->size().width(), m_newWaterfallFrameBuffer->size().height());
    //f->glViewport(m_viewPortBottomLeft.x(), m_viewPortBottomLeft.y(), m_viewPortSize.first, m_viewPortSize.second);
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glBlendEquation(GL_FUNC_ADD);
    m_projection.setToIdentity();
    m_projection2.setToIdentity();

    if (m_zoomAreaSelected)
    {
        if (m_chartType == HorizantalWaterfall)
        {
            m_projection.ortho(m_zoomedData.z(), m_zoomedData.w(), m_zoomedData.x(), m_zoomedData.y(), -20.0f, 20.0f);
            m_projection2.ortho(0.0f /** 2.0f*/, 600.0f/* * 2.0f*/, m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
        }
        else
        {
            m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
            m_projection2.ortho(m_zoomedData.x(), m_zoomedData.y(), 0.0f /** 2.0f*/, 600.0f/* * 2.0f*/, -20.0f, 20.0f);
        }
    }
    else
    {
        if (m_chartType == Waterfall)
        {
            m_ChartHorizantalRange = getChartHorizantalRange();
            m_ChartVerticalRange = getChartVerticalRange();

            QVector2D rangeX = m_ChartHorizantalRange;
            QVector2D rangeY = m_ChartVerticalRange;

            m_zoomedData.setX(rangeX.x());
            m_zoomedData.setY(rangeX.y());
            m_zoomedData.setZ(rangeY.x());
            m_zoomedData.setW(rangeY.y());

            m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
            m_projection2.ortho(m_zoomedData.x(), m_zoomedData.y(), 0.0f /** 2.0f*/, 600.0f/* * 2.0f*/, -20.0f, 20.0f);
        }
        else if (m_chartType == HorizantalWaterfall)
        {
            m_ChartHorizantalRange = getChartHorizantalRange();
            m_ChartVerticalRange = getChartVerticalRange();

            QVector2D rangeX = m_ChartHorizantalRange;
            QVector2D rangeY = m_ChartVerticalRange;

            m_zoomedData.setX(rangeX.x());
            m_zoomedData.setY(rangeX.y());
            m_zoomedData.setZ(rangeY.x());
            m_zoomedData.setW(rangeY.y());

            //m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
            m_projection.ortho(m_zoomedData.z(), m_zoomedData.w(), m_zoomedData.x(), m_zoomedData.y(), -20.0f, 20.0f);
            m_projection2.ortho(0.0f /** 2.0f*/, 600.0f/* * 2.0f*/, m_zoomedData.z(), m_zoomedData.w(),  -20.0f, 20.0f);
        }
    }


    m_spectrumProgram->bind();
    m_spectrumObject.bind();

    m_spectrumBuffer.bind();
    //m_spectrumCfarBuffer.bind();
    //if (next)
    //{
    //	//m_camera2.rotate(60.0,1,0,0);
    //	//m_transform2.translate(0, TimeSpectrum, -5.0f);
    //	m_transform2.translate(0.0f, +10.0f, -5.0f);
    //}
    m_spectrumProgram->setUniformValue(u_spectrumWorldToCamera, m_camera.toMatrix());
    m_spectrumProgram->setUniformValue(u_spectrumCameraToView, m_projection);
    m_spectrumProgram->setUniformValue(u_spectrumCameraToView2, m_projection2);
    m_spectrumProgram->setUniformValue(u_spectrumModelToWorld, m_transform.toMatrix());
    m_spectrumProgram->setUniformValue(u_spectrumColor, series->seriesColor());
    //m_spectrumProgram->setUniformValue(u_spectrumColor, m_spectrumColor);
    if (m_chartType == Waterfall)
    {
        m_spectrumProgram->setUniformValue(u_spectrumTime, 597.0f/*TimeSpectrum*/);
        m_spectrumProgram->setUniformValue(u_spectrumLineWidth, 7.0f);
        m_spectrumProgram->setUniformValue(u_spectrumWindowScale, QVector2D((float)m_viewPortSize.first, (float)m_viewPortSize.second));
    }
    else if (m_chartType == HorizantalWaterfall)
    {
        m_spectrumProgram->setUniformValue(u_spectrumTime, 3.0f/*TimeSpectrum*/);
    }

    m_spectrumBuffer.allocate(series->pointsVector().constData(), series->seriesSize() * sizeof(GLfloat) * 2);

    m_spectrumProgram->enableAttributeArray(0);
    //m_spectrumCfarBuffer.bind();
    m_spectrumProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

    f->glDisable(GL_LINE_SMOOTH);
    //f->glLineWidth(7);
    f->glDrawArrays(GL_LINE_STRIP, 0, series->seriesSize());
    //f->glLineWidth(1);
    m_spectrumObject.release();
    m_spectrumBuffer.release();
    m_spectrumProgram->release();
    f->glDisable(GL_BLEND);

    //TimeSpectrum++;
    //if (TimeSpectrum == 600)
    //{
    //	//next = true;
    //	TimeSpectrum = 0.0;
    //}
    //m_oldWaterfallFrameBuffer->release();

}

////////////////// Draws the zoom rectangle /////////////////

void QGLchart::initializeZoomAreaProgram()
{
    //Create Shader (DO not release unit VAO is created)
    m_zoomAreaProgram = new QOpenGLShaderProgram();
    m_zoomAreaProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/zoomArea.vert");
    m_zoomAreaProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/zoomArea.frag");
    m_zoomAreaProgram->link();
    m_zoomAreaProgram->bind();

    // Cache Uniform Locations
    u_zoomAreaModelToWorld = m_zoomAreaProgram->uniformLocation("modelToWorld");
    u_zoomAreaWorldToCamera = m_zoomAreaProgram->uniformLocation("worldToCamera");
    u_zoomAreaCameraToView = m_zoomAreaProgram->uniformLocation("cameraToView");
    u_zoomAreaColor = m_zoomAreaProgram->uniformLocation("color");
    //Create Buffer (Do not release until VAO is created)


    //Create Vertex Array Object
    m_zoomAreaObject.create();
    m_zoomAreaObject.bind();

    m_zoomAreaBuffer.create();
    m_zoomAreaBuffer.bind();
    m_zoomAreaBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_zoomAreaBuffer.allocate(NULL, m_zoomAreaPosition.count() * sizeof(GLfloat) * 2);

    m_zoomAreaProgram->enableAttributeArray(0);
    m_zoomAreaBuffer.bind();
    m_zoomAreaProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);


    // Release (unbind) all
    m_zoomAreaBuffer.release();
    m_zoomAreaObject.release();
    m_zoomAreaProgram->release();

}

void QGLchart::updateZoomAreaProgram(ZoomType zoomType)
{
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glBlendEquation(GL_FUNC_ADD);
    //calculateViewPort(m_geometry, m_contentsMargin);
    f->glViewport(m_viewPortBottomLeft.x(), m_viewPortBottomLeft.y(), m_viewPortSize.first, m_viewPortSize.second);
    m_projection.setToIdentity();

    m_projection.ortho(m_drawRegion.left(), m_drawRegion.right(), m_drawRegion.bottom(), m_drawRegion.top(), -20.0f, 20.0f);

    m_zoomAreaProgram->bind();
    m_zoomAreaObject.bind();
    m_zoomAreaBuffer.bind();

    m_zoomAreaProgram->setUniformValue(u_zoomAreaWorldToCamera, m_camera.toMatrix());
    m_zoomAreaProgram->setUniformValue(u_zoomAreaCameraToView, m_projection);
    m_zoomAreaProgram->setUniformValue(u_zoomAreaModelToWorld, m_transform.toMatrix());
    QColor zoomAreaColor = QColor(255, 255, 255, 150);
    if (m_areaSelectionState)
        zoomAreaColor = QColor(255, 255, 0, 200);
    m_zoomAreaProgram->setUniformValue(u_zoomAreaColor, zoomAreaColor);
    m_zoomAreaPosition.clear();

    QRect zoomAreaRectangle = m_zoomAreaRectangle;
    zoomAreaRectangle.translate(m_drawRegion.topLeft());
    m_zoomAreaBuffer.bind();
    if (zoomType == HorizantalZoom)
    {
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.topLeft().x(), zoomAreaRectangle.topLeft().y()));
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.bottomLeft().x(), zoomAreaRectangle.bottomLeft().y()));
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.topRight().x(), zoomAreaRectangle.topRight().y()));
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.bottomRight().x(), zoomAreaRectangle.bottomRight().y()));
        m_zoomAreaPosition.append((m_zoomAreaPosition.at(0) + m_zoomAreaPosition.at(1)) / 2);
        m_zoomAreaPosition.append((m_zoomAreaPosition.at(2) + m_zoomAreaPosition.at(3)) / 2);
    }
    else if (zoomType == VerticalZoom)
    {
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.topLeft().x(), zoomAreaRectangle.topLeft().y()));
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.topLeft().x() + 10, zoomAreaRectangle.topLeft().y()));
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.bottomLeft().x(), zoomAreaRectangle.bottomLeft().y()));
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.bottomLeft().x() + 10, zoomAreaRectangle.bottomLeft().y()));
        m_zoomAreaPosition.append((m_zoomAreaPosition.at(0) + m_zoomAreaPosition.at(1)) / 2);
        m_zoomAreaPosition.append((m_zoomAreaPosition.at(2) + m_zoomAreaPosition.at(3)) / 2);
    }
    else if (zoomType == RectangleZoom)
    {
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.topLeft().x(), zoomAreaRectangle.topLeft().y()));
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.topRight().x(), zoomAreaRectangle.topRight().y()));
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.bottomRight().x(), zoomAreaRectangle.bottomRight().y()));
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.bottomLeft().x(), zoomAreaRectangle.bottomLeft().y()));
        m_zoomAreaPosition.append(QVector2D(zoomAreaRectangle.topLeft().x(), zoomAreaRectangle.topLeft().y()));
    }

    m_zoomAreaBuffer.allocate(m_zoomAreaPosition.constData(), m_zoomAreaPosition.count() * sizeof(GLfloat) * 2);
    m_zoomAreaProgram->enableAttributeArray(0);
    m_zoomAreaBuffer.bind();
    m_zoomAreaProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);
    f->glEnable(GL_LINE_SMOOTH);
    if (m_areaSelectionState)
        f->glLineWidth(2);
    else if (m_zoomState)
        f->glLineWidth(2);
    if (zoomType == RectangleZoom)
    {
        f->glDrawArrays(GL_LINE_STRIP, 0, m_zoomAreaPosition.count());
    }
    else
    {
        f->glDrawArrays(GL_LINES, 0, m_zoomAreaPosition.count());
    }

    m_zoomAreaObject.release();
    m_zoomAreaBuffer.release();
    m_zoomAreaProgram->release();
    f->glDisable(GL_BLEND);
}

void QGLchart::initializeTextProgram()
{
    // Compile and setup the shader
    m_textProgram = new QOpenGLShaderProgram();
    m_textProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/text.vert");
    m_textProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/text.frag");
    m_textProgram->link();
    m_textProgram->bind();

    // Cache Uniform Locations
    u_textModelToWorld = m_textProgram->uniformLocation("modelToWorld");
    u_textWorldToCamera = m_textProgram->uniformLocation("worldToCamera");
    u_textCameraToView = m_textProgram->uniformLocation("cameraToView");
    //u_textScale = m_textProgram->uniformLocation("scale");
    u_textColor = m_textProgram->uniformLocation("color");

    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // Load font as face
    FT_Face face;

#ifdef LINUX

    QString pth = QDir::currentPath()+"/font/arial.ttf";
    QByteArray ba = pth.toLocal8Bit();
    const char *dirPath = ba.data();

    if (FT_New_Face(ft, dirPath, 0, &face))
   // if (FT_New_Face(ft, "./font/arial.ttf", 0, &face))
         std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
#else
    if (FT_New_Face(ft, "C:/Windows/Fonts/arial.ttf", 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
#endif
    // Set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 35);

    // Disable byte-alignment restriction
    f->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        f->glGenTextures(1, &texture);
        f->glBindTexture(GL_TEXTURE_2D, texture);
        f->glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
            );
        // Set texture options
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            QVector2D(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            QVector2D(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<GLuint>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    f->glBindTexture(GL_TEXTURE_2D, 0);
    // Destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


    // Configure VAO/VBO for texture quads
    m_textObject.create();
    m_textBuffer.create();
    m_textObject.bind();
    m_textBuffer.bind();
    m_textBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_textBuffer.allocate(NULL, sizeof(GLfloat) * 6 * 4);

    m_textProgram->enableAttributeArray(0);
    m_textBuffer.bind();
    m_textProgram->setAttributeBuffer(0, GL_FLOAT, 0, 4, 4 * sizeof(GLfloat));

    m_textBuffer.release();
    m_textObject.release();
    m_textProgram->release();
}

void QGLchart::RenderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, QVector3D color)
{
    f->glViewport(0, 0, m_geometry.width(), m_geometry.height());

    m_projection.setToIdentity();
    m_projection.ortho(0, m_geometry.width(), 0, m_geometry.height(), -100, 100);

    // Activate corresponding render state
    m_textProgram->bind();
    m_textProgram->setUniformValue(u_textWorldToCamera, m_camera.toMatrix());
    m_textProgram->setUniformValue(u_textCameraToView, m_projection);
    m_textProgram->setUniformValue(u_textModelToWorld, m_transform.toMatrix());
    m_textProgram->setUniformValue(u_textColor, color);
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    f->glEnable(GL_TEXTURE_2D);
    f->glActiveTexture(GL_TEXTURE0);
    m_textObject.bind();

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x() * scale;
        GLfloat ypos = y - (ch.Size.y() - ch.Bearing.y()) * scale;

        GLfloat w = ch.Size.x() * scale;
        GLfloat h = ch.Size.y() * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos, ypos + h, 0.0, 0.0 },
            { xpos, ypos, 0.0, 1.0 },
            { xpos + w, ypos, 1.0, 1.0 },

            { xpos, ypos + h, 0.0, 0.0 },
            { xpos + w, ypos, 1.0, 1.0 },
            { xpos + w, ypos + h, 1.0, 0.0 }
        };
        // Render glyph texture over quad
        f->glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        m_textBuffer.bind();
        f->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        m_textBuffer.release();
        // Render quad
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    m_textObject.release();
    m_textProgram->release();
    f->glBindTexture(GL_TEXTURE_2D, 0);
    f->glDisable(GL_TEXTURE_2D);
    f->glDisable(GL_BLEND);

}

void QGLchart::RenderTextVertical(std::string text, GLfloat x, GLfloat y, GLfloat scale, QVector3D color, Qt::Alignment alignment)
{
    f->glViewport(0, 0, m_geometry.width(), m_geometry.height());

    float rotationAngle;
    if (alignment == Qt::AlignLeft)
        rotationAngle = -90.0f;
    else if (alignment == Qt::AlignRight)
        rotationAngle = 90.0f;

    m_projection.setToIdentity();
    m_projection.ortho(0, m_geometry.width(), 0, m_geometry.height(), -100, 100);
    m_camera.rotate(rotationAngle, 0.0f, 0.0f, 1.0f);

    // Activate corresponding render state
    m_textProgram->bind();
    m_textProgram->setUniformValue(u_textWorldToCamera, m_camera.toMatrix());
    m_textProgram->setUniformValue(u_textCameraToView, m_projection);
    m_textProgram->setUniformValue(u_textModelToWorld, m_transform.toMatrix());
    m_textProgram->setUniformValue(u_textColor, color);
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    f->glEnable(GL_TEXTURE_2D);
    f->glActiveTexture(GL_TEXTURE0);
    m_textObject.bind();

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = x + ch.Bearing.x() * scale;
        GLfloat ypos = y - (ch.Size.y() - ch.Bearing.y()) * scale;

        GLfloat w = ch.Size.x() * scale;
        GLfloat h = ch.Size.y() * scale;
        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos, ypos + h, 0.0, 0.0 },
            { xpos, ypos, 0.0, 1.0 },
            { xpos + w, ypos, 1.0, 1.0 },

            { xpos, ypos + h, 0.0, 0.0 },
            { xpos + w, ypos, 1.0, 1.0 },
            { xpos + w, ypos + h, 1.0, 0.0 }
        };
        // Render glyph texture over quad
        f->glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        m_textBuffer.bind();
        f->glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        m_textBuffer.release();
        // Render quad
        f->glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    m_textObject.release();
    m_textProgram->release();
    f->glBindTexture(GL_TEXTURE_2D, 0);
    f->glDisable(GL_TEXTURE_2D);
    f->glDisable(GL_BLEND);
    m_camera.rotate(-rotationAngle, 0.0f, 0.0f, 1.0f);

}

/****************************************************************************************/
/*							   Axis Label Plotting Functions							*/
/****************************************************************************************/

void QGLchart::drawChartTitle()
{
    RenderText(m_titleText.toLocal8Bit().constData(), m_geometry.width() / 2 - (m_geometry.width() * 0.12),
        (m_geometry.height() * 0.97), 0.42f, qColorToQVector3D(m_titleBrush.color()));
}

void QGLchart::drawHorizontalLabel()
{
    RenderText(m_horizantalAxes->titleText().toLocal8Bit().constData(), m_geometry.width() / 2 - (m_geometry.width() * 0.12),
        (m_geometry.height() * 0.01), 0.32f, qColorToQVector3D(m_horizantalAxes->titleBrush().color()));

}

void QGLchart::drawVerticalLabel()
{
    RenderTextVertical(m_verticalAxes->titleText().toLocal8Bit().constData(), m_geometry.height() / 2.0 - (m_geometry.height() * m_verticalLebelRelPos.y()),
        -(m_geometry.width() * m_verticalLebelRelPos.x()), 0.32f, qColorToQVector3D(m_verticalAxes->titleBrush().color()));
}

void QGLchart::drawHorizontalScaleLabel()
{
    if (!m_manualHorzScalePosEnabled)
        RenderText(m_horizantalAxes->labelsText().toLocal8Bit().constData(),m_geometry.width() / 2 + (m_geometry.width() * 0.05), (m_geometry.height() * 0.01), 0.32f, QVector3D(0.0, 1.0f, 0.0f));
    else
        RenderText(m_horizantalAxes->labelsText().toLocal8Bit().constData(), m_geometry.width() / 2 + (m_geometry.width() * m_manualHorzScalePos.x()), (m_geometry.height() * m_manualHorzScalePos.y()), 0.42f, QVector3D(0.0, 1.0f, 0.0f));
}

void QGLchart::drawVerticalScaleLabel()		// such as dbm
{
    RenderTextVertical(m_verticalAxes->labelsText().toLocal8Bit().constData(), m_geometry.height() / 2.0 + (m_geometry.height() * 0.05), -(m_geometry.width() * m_verticalLebelRelPos.x()), 0.32f, QVector3D(0.0, 1.0f, 0.0f));
}

/****************************************************************************************/
/*						   Axis related functions and variables							*/
/****************************************************************************************/

void QGLchart::setHorizontalNumberCount(int hNumberCount)
{
    m_hNumberCount = hNumberCount;
}

int QGLchart::getHorizontalNumberCount() const
{
    return m_hNumberCount;
}

void QGLchart::setVerticalRangeOffset(QVector2D rangeOffset)
{
    m_ChartVerticalRangeOffset = rangeOffset;
}

QVector2D QGLchart::getVertcialRangeOffset() const
{
    return m_ChartVerticalRangeOffset;
}

/****************************************************************************************/
/*								 Masking Plotting Functions								*/
/****************************************************************************************/

void QGLchart::initializeMaskingProgram()
{
    //Create Shader (DO not release unit VAO is created)
    m_maskingProgram = new QOpenGLShaderProgram();
    m_maskingProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/masking.vert");
    m_maskingProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/masking.frag");
    m_maskingProgram->link();
    m_maskingProgram->bind();

    // Cache Uniform Locations
    u_maskingModelToWorld = m_maskingProgram->uniformLocation("modelToWorld");
    u_maskingWorldToCamera = m_maskingProgram->uniformLocation("worldToCamera");
    u_maskingCameraToView = m_maskingProgram->uniformLocation("cameraToView");
    u_maskingColor = m_maskingProgram->uniformLocation("boxColor");
    //Create Buffer (Do not release until VAO is created)

    //Create Vertex Array Object
    m_maskingObject.create();
    m_maskingObject.bind();

    m_maskingBuffer.create();
    m_maskingBuffer.bind();
    m_maskingBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_maskingBuffer.allocate(m_boxMaskPosition.constData(), m_boxMaskPosition.count() * sizeof(GLfloat) * 2);

    m_maskCenterBuffer.create();
    m_maskCenterBuffer.bind();
    m_maskCenterBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_maskCenterBuffer.allocate(NULL, m_maskingCenterPoints.count() * sizeof(GLfloat) * 2);

    m_maskingProgram->enableAttributeArray(0);
    m_maskingBuffer.bind();
    m_maskingProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

    m_maskingProgram->enableAttributeArray(1);
    m_maskCenterBuffer.bind();
    m_maskingProgram->setAttributeBuffer(1, GL_FLOAT, 0, 2, 0);

    // Release (unbind) all
    m_maskingBuffer.release();
    m_maskCenterBuffer.release();
    m_maskingObject.release();
    m_maskingProgram->release();

}

void QGLchart::updateMaskingProgram()
{
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glBlendEquation(GL_FUNC_ADD);
    //calculateViewPort(m_geometry, m_contentsMargin);
    f->glViewport(m_viewPortBottomLeft.x(), m_viewPortBottomLeft.y(), m_viewPortSize.first, m_viewPortSize.second);
    m_projection.setToIdentity();

    if (m_zoomAreaSelected)
    {
        m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), -1.0f, 1.0f, -20.0f, 20.0f);
    }
    else
    {
        QVector2D rangeX = getChartHorizantalRange();

        m_zoomedData.setX(rangeX.x());
        m_zoomedData.setY(rangeX.y());

        m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), -1.0f, 1.0f, -20.0f, 20.0f);
    }

    m_maskingProgram->bind();
    m_maskingObject.bind();
    m_maskingBuffer.bind();

    m_maskingProgram->setUniformValue(u_maskingWorldToCamera, m_camera.toMatrix());
    m_maskingProgram->setUniformValue(u_maskingCameraToView, m_projection);
    m_maskingProgram->setUniformValue(u_maskingModelToWorld, m_transform.toMatrix());
    m_maskingProgram->setUniformValue(u_maskingColor, m_maskingColor);

    m_maskCenterBuffer.bind();
    m_maskCenterBuffer.allocate(m_maskingCenterPoints.constData(), m_maskingCenterPoints.count() * sizeof(GLfloat) * 2);
    m_maskingProgram->enableAttributeArray(0);
    m_maskingBuffer.bind();
    m_maskingProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

    m_maskingProgram->enableAttributeArray(1);
    m_maskCenterBuffer.bind();
    m_maskingProgram->setAttributeBuffer(1, GL_FLOAT, 0, 2, 0);

    f->glVertexAttribDivisor(0, 0);
    f->glVertexAttribDivisor(1, 1);
    f->glEnable(GL_LINE_SMOOTH);
    f->glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_maskingCenterPoints.count());

    m_maskingObject.release();
    m_maskingBuffer.release();
    m_maskCenterBuffer.release();
    m_maskingProgram->release();
    f->glDisable(GL_BLEND);
}

/****************************************************************************************/
/*								 Threshold Plotting Functions							*/
/****************************************************************************************/

void QGLchart::initializeThresholdProgram()
{
    //Create Shader (DO not release unit VAO is created)
    m_thresholdProgram = new QOpenGLShaderProgram();
    m_thresholdProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/threshold.vert");
    m_thresholdProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/threshold.frag");
    m_thresholdProgram->link();
    m_thresholdProgram->bind();

    // Cache Uniform Locations
    u_thresholdModelToWorld = m_thresholdProgram->uniformLocation("modelToWorld");
    u_thresholdWorldToCamera = m_thresholdProgram->uniformLocation("worldToCamera");
    u_thresholdCameraToView = m_thresholdProgram->uniformLocation("cameraToView");
    u_thresholdColor = m_thresholdProgram->uniformLocation("color");
    //Create Buffer (Do not release until VAO is created)


    //Create Vertex Array Object
    m_thresholdObject.create();
    m_thresholdObject.bind();

    m_thresholdBuffer.create();
    m_thresholdBuffer.bind();
    m_thresholdBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_thresholdBuffer.allocate(NULL, m_thresholdPosition.count() * sizeof(GLfloat) * 2);

    m_thresholdProgram->enableAttributeArray(0);
    m_thresholdBuffer.bind();
    m_thresholdProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);


    // Release (unbind) all
    m_thresholdBuffer.release();
    m_thresholdObject.release();
    m_thresholdProgram->release();

}

void QGLchart::updateThresholdProgram()
{
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glBlendEquation(GL_FUNC_ADD);
    //calculateViewPort(m_geometry, m_contentsMargin);
    f->glViewport(m_viewPortBottomLeft.x(), m_viewPortBottomLeft.y(), m_viewPortSize.first, m_viewPortSize.second);
    m_projection.setToIdentity();

    if (m_zoomAreaSelected)
    {
        m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
    }
    else
    {
        QVector2D rangeX = getChartHorizantalRange();
        QVector2D rangeY = getChartVerticalRange();

        m_zoomedData.setX(rangeX.x());
        m_zoomedData.setY(rangeX.y());
        m_zoomedData.setZ(rangeY.x());
        m_zoomedData.setW(rangeY.y());

        m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
    }

    //if (!m_addedSeriesData.isEmpty())
    //{
    //	int totalDataLength = 0;
    //	for (int i = 0; i < m_addedSeriesData.length(); i++)
    //		totalDataLength += m_addedSeriesData.at(i).seriesSize();
    //	if (totalDataLength > 0)
    //	{
    //		if (m_zoomAreaSelected)
    //		{
    //			m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
    //		}
    //		else
    //		{
    //			QVector2D rangeX = getChartHorizantalRange();
    //			QVector2D rangeY = getChartVerticalRange();

    //			m_zoomedData.setX(rangeX.x());
    //			m_zoomedData.setY(rangeX.y());
    //			m_zoomedData.setZ(rangeY.x());
    //			m_zoomedData.setW(rangeY.y());

    //			m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
    //			//m_projection.ortho(rangeX.x(), rangeX.y(), rangeY.x(), rangeY.y(), -20.0f, 20.0f);
    //		}
    //	}
    //	else
    //	{
    //		m_projection.ortho(0.0f, m_modifiableSectionsNumber + 1, -1.0f, 1.0f, -20.0f, 20.0f);
    //	}
    //}
    //else
    //{
    //	m_projection.ortho(0.0f, m_modifiableSectionsNumber + 1, -1.0f, 1.0f, -20.0f, 20.0f);
    //}

    m_thresholdProgram->bind();
    m_thresholdObject.bind();
    m_thresholdBuffer.bind();

    m_thresholdProgram->setUniformValue(u_thresholdWorldToCamera, m_camera.toMatrix());
    m_thresholdProgram->setUniformValue(u_thresholdCameraToView, m_projection);
    m_thresholdProgram->setUniformValue(u_thresholdModelToWorld, m_transform.toMatrix());
    m_thresholdProgram->setUniformValue(u_thresholdColor, m_thresholdColor);
    m_thresholdPosition.clear();

    m_thresholdBuffer.bind();
    m_thresholdPosition = m_thresholdPoints;

    m_thresholdBuffer.allocate(m_thresholdPosition.constData(), m_thresholdPosition.count() * sizeof(GLfloat) * 2);
    m_thresholdProgram->enableAttributeArray(0);
    m_thresholdBuffer.bind();
    m_thresholdProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

    f->glEnable(GL_LINE_SMOOTH);
    f->glDrawArrays(GL_LINE_STRIP, 0, m_thresholdPosition.count());

    m_thresholdObject.release();
    m_thresholdBuffer.release();
    m_thresholdProgram->release();
    f->glDisable(GL_BLEND);
}

/****************************************************************************************/
/*							Drawing and the setup of Axis Numbers						*/
/****************************************************************************************/

void QGLchart::setVerticalNumberPrecision(int precision)
{
    m_verticalNumberPrecision = precision;
}

int QGLchart::getVerticalNumberPrecision()
{
    return m_verticalNumberPrecision;
}

void QGLchart::setHorizantalNumberPrecision(int precision)
{
    m_HorizantalNumberPrecision = precision;
}

int QGLchart::getHorizantalNumberPrecision()
{
    return m_HorizantalNumberPrecision;
}

void QGLchart::drawHorizontalNumbers()
{
    float hNumber;
    float lowerRange, higherRange, stepSize;
    int numLength = 0;
    float hNumberCount = (float)m_hNumberCount;

    if (m_viewPortSize.first < 350 && hNumberCount > 5)		// temporary
        hNumberCount = 5.0;

    lowerRange = m_zoomedData.x();
    higherRange = m_zoomedData.y();
    stepSize = (higherRange - lowerRange) / hNumberCount;

    for (int i = 0; i <= hNumberCount; i++)
    {
        hNumber = lowerRange + (i * stepSize);
        if (m_horizantalAxes->scaleType() == ScaleType::Logrithmic)
            hNumber = pow(10, hNumber);
        //numLength = QString::number(hNumber, 'f', m_HorizantalNumberPrecision).toStdString().size();
        numLength = QString::number(hNumber).toStdString().size();
        RenderText(QString::number(hNumber).toStdString(), m_viewPortBottomLeft.x() + (i * (m_viewPortSize.first / hNumberCount)) - numLength * 5,
            (m_viewPortBottomLeft.y() - 15), 0.32f, QVector3D(1.0f, 1.0f, 1.0f));
        /*RenderText(QString::number(hNumber, 'f', m_HorizantalNumberPrecision).toStdString(), m_viewPortBottomLeft.x() + (i * (m_viewPortSize.first / 8.0)) - numLength * 5,
            (m_viewPortBottomLeft.y() - (m_viewPortSize.second * 0.09)), 0.42f, QVector3D(0.0, 1.0f, 0.0f));*/
        /*RenderText(QString::number(hNumber, 'f', m_HorizantalNumberPrecision).toStdString(), m_viewPortBottomLeft.x() + (i * (m_viewPortSize.first / 8.0)) - numLength * 5,
            (m_geometry.height() * 0.05), 0.42f, QVector3D(0.0, 1.0f, 0.0f));*/
    }
}

void QGLchart::drawVerticalNumbers()
{
    float vNumber;
    float lowerRange, higherRange, stepSize;
    int numLength = 0;
    float vNumberCount = 8.0;

    if (((float)m_viewPortSize.second / m_viewPortSize.first) < 0.2)
        vNumberCount = 5.0;

    lowerRange = m_zoomedData.z();
    higherRange = m_zoomedData.w();
    stepSize = (higherRange - lowerRange) / vNumberCount;
    QVector<QString> numberList(vNumberCount + 1);
    int maxNumberLength = 0;

    for (int i = 0; i <= vNumberCount; i++)
    {
        vNumber = lowerRange + (i * stepSize);
        if (m_verticalAxes->scaleType() == ScaleType::Logrithmic)
            vNumber = pow(10, vNumber);

        numberList[i] = QString::number(vNumber, 'f', m_verticalNumberPrecision);
    }

    QVector<QPair<QString, qreal> > newNumberList = yAxisNumberTuner(numberList);

    for (int i = 0; i <= vNumberCount; i++)
    {
        RenderText(newNumberList.at(i).first.toStdString(), (m_viewPortBottomLeft.x() - 3 - newNumberList.at(i).second), m_viewPortBottomLeft.y() + (i * (m_viewPortSize.second / vNumberCount)) - 0.5,
            0.32f, QVector3D(1.0, 1.0f, 1.0f));
    }
}

QVector<QPair<QString, qreal> > QGLchart::yAxisNumberTuner(QVector<QString> inputNumbers)
{
    double desiredDistance = m_viewPortBottomLeft.x() - 2 - (m_geometry.width() * m_verticalLebelRelPos.x());
    QVector<QPair<QString, qreal> > outputNumbers(inputNumbers.size());

    for (int i = 0; i < inputNumbers.size(); i++)
    {
        std::string::const_iterator c;
        double labelWidth = 0.0;
        std::string currentNumber = inputNumbers.at(i).toStdString();
        for (c = currentNumber.begin(); c != currentNumber.end(); c++)
        {
            Character ch = Characters[*c];
            labelWidth += (ch.Advance >> 6) * 0.42f;
        }

        /*outputNumbers[i].first = inputNumbers.at(i);
        outputNumbers[i].second = labelWidth;
        if (labelWidth > desiredDistance)
        {
            double diff = labelWidth - desiredDistance;

        }*/
        if ((labelWidth <= desiredDistance) || (!m_applyFineTuning))
        {
            outputNumbers[i].first = inputNumbers.at(i);
            outputNumbers[i].second = labelWidth;
        }
        else
        {
            double labelNumber = inputNumbers.at(i).toDouble();
            outputNumbers[i].first = QString::number(labelNumber, 'e', m_verticalNumberPrecision);

            double newLabelWidth = 0;
            std::string currentLongNumber = outputNumbers[i].first.toStdString();
            for (c = currentLongNumber.begin(); c != currentLongNumber.end(); c++)
            {
                Character ch = Characters[*c];
                newLabelWidth += (ch.Advance >> 6) * 0.42f;
            }
            if (newLabelWidth < desiredDistance)
                outputNumbers[i].second = newLabelWidth;
            else
                outputNumbers[i].second = desiredDistance - 2;
        }

    }

    return outputNumbers;
}

/****************************************************************************************/
/*								 Yaxis2 Plotting Functions								*/
/****************************************************************************************/

void QGLchart::drawYaxis2Numbers()
{
    float vNumber;
    float lowerRange, higherRange, stepSize;
    int numLength = 0;
    float vNumberCount = 4.0;

    if (((float)m_viewPortSize.second / m_viewPortSize.first) < 0.2)
        vNumberCount = 3.0;

    lowerRange = yAxis2->getRange().first;
    higherRange = yAxis2->getRange().second;
    stepSize = (higherRange - lowerRange) / vNumberCount;
    QVector<QString> numberList(vNumberCount + 1);
    int maxNumberLength = 0;

    for (int i = 0; i <= vNumberCount; i++)
    {
        if (yAxis2->rangeReversed())
            vNumber = lowerRange + ((vNumberCount - i) * stepSize);
        else
            vNumber = lowerRange + (i * stepSize);
        RenderText(QString::number(vNumber, 'f', yAxis2->numberPrecision()).toStdString(), (m_viewPortBottomLeft.x() + m_viewPortSize.first + yAxis2->tickLabelPadding()),
            m_viewPortBottomLeft.y() + (i * (m_viewPortSize.second / vNumberCount)) - 3.5, 0.32f, QVector3D(0.0, 1.0f, 0.0f));
    }
}

void QGLchart::drawYaxis2Text()
{
    RenderTextVertical(yAxis2->titleText().toLocal8Bit().constData(), m_geometry.height() / 2.0 - (m_geometry.height() * 0.10),
        -(m_geometry.width() * 0.975), 0.32f, qColorToQVector3D(m_verticalAxes->titleBrush().color()));
}

void QGLchart::drawYaxis2LabelText()
{
    RenderTextVertical(yAxis2->labelsText().toLocal8Bit().constData(), m_geometry.height() / 2.0 + (m_geometry.height() * 0.05), -(m_geometry.width() * 0.975), 0.32f, QVector3D(0.0, 1.0f, 0.0f));
}

/****************************************************************************************/
/*								  Marker Plotting Functions								*/
/****************************************************************************************/

void QGLchart::initializeMarkerProgram()
{
    //Create Shader (DO not release unit VAO is created)
    m_markerProgram = new QOpenGLShaderProgram();
    m_markerProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/Marker.vert");
    m_markerProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/Marker.frag");
    m_markerProgram->link();
    m_markerProgram->bind();

    // Cache Uniform Locations
    u_MarkerModelToWorld	=	m_markerProgram->uniformLocation("modelToWorld");
    u_MarkerWorldToCamera	=	m_markerProgram->uniformLocation("worldToCamera");
    u_MarkerCameraToView	=	m_markerProgram->uniformLocation("cameraToView");
    u_MarkerColor			=	m_markerProgram->uniformLocation("color");
    u_xMarkerPlotScale		=	m_markerProgram->uniformLocation("xscale");
    u_yMarkerPlotScale		=	m_markerProgram->uniformLocation("yscale");
    //Create Buffer (Do not release until VAO is created)


    //Create Vertex Array Object
    m_markerObject.create();
    m_markerObject.bind();

    m_markerBuffer.create();
    m_markerBuffer.bind();
    m_markerBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_markerBuffer.allocate(m_markerPosition.constData(), m_markerPosition.count() * sizeof(GLfloat) * 2);

    m_markerCenterBuffer.create();
    m_markerCenterBuffer.bind();
    m_markerCenterBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_markerCenterBuffer.allocate(NULL, m_markerCenterPoint.count() * sizeof(GLfloat) * 2);

    m_markerProgram->enableAttributeArray(0);
    m_markerBuffer.bind();
    m_markerProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

    m_markerProgram->enableAttributeArray(1);
    m_markerCenterBuffer.bind();
    m_markerProgram->setAttributeBuffer(1, GL_FLOAT, 0, 2, 0);


    // Release (unbind) all
    m_markerBuffer.release();
    m_markerCenterBuffer.release();
    m_markerObject.release();
    m_markerProgram->release();
}

void QGLchart::updateMarkerProgram()
{
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glBlendEquation(GL_FUNC_ADD);

    f->glViewport(m_viewPortBottomLeft.x(), m_viewPortBottomLeft.y(), m_viewPortSize.first, m_viewPortSize.second);
    m_projection.setToIdentity();

    if (m_zoomAreaSelected)
    {
        m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
    }
    else
    {
        QVector2D rangeX = m_ChartHorizantalRange;
        QVector2D rangeY = m_ChartVerticalRange;

        m_zoomedData.setX(rangeX.x());
        m_zoomedData.setY(rangeX.y());
        m_zoomedData.setZ(rangeY.x());
        m_zoomedData.setW(rangeY.y());

        m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
    }

    m_markerProgram->bind();
    m_markerObject.bind();
    m_markerBuffer.bind();

    m_markerProgram->setUniformValue(u_MarkerWorldToCamera, m_camera.toMatrix());
    m_markerProgram->setUniformValue(u_MarkerCameraToView, m_projection);
    m_markerProgram->setUniformValue(u_MarkerModelToWorld, m_transform.toMatrix());
    m_markerProgram->setUniformValue(u_MarkerColor, m_markerColor);

    updateMarkerCenterPoints();
    m_markerCenterBuffer.bind();
    m_markerCenterBuffer.allocate(m_markerCenterPoint.constData(), m_markerCenterPoint.count() * sizeof(GLfloat) * 2);

    m_markerProgram->setUniformValue(u_xMarkerPlotScale, calculateScatterScale(m_viewPortSize.first, (m_zoomedData.y() - m_zoomedData.x())));
    m_markerProgram->setUniformValue(u_yMarkerPlotScale, calculateScatterScale(m_viewPortSize.second, (m_zoomedData.w() - m_zoomedData.z())));
    m_markerProgram->enableAttributeArray(0);
    m_markerBuffer.bind();
    m_markerProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

    m_markerProgram->enableAttributeArray(1);
    m_markerCenterBuffer.bind();
    m_markerCenterBuffer.setUsagePattern(QOpenGLBuffer::StreamDraw);
    m_markerProgram->setAttributeBuffer(1, GL_FLOAT, 0, 2, 0);

    f->glVertexAttribDivisor(0, 0);
    f->glVertexAttribDivisor(1, 1);
    f->glEnable(GL_LINE_SMOOTH);
    f->glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_markerCenterPoint.count());

    m_markerObject.release();
    m_markerCenterBuffer.release();
    m_markerProgram->release();
    //m_markerBuffer.release();
    f->glDisable(GL_BLEND);
    //f->glViewport(0, 0, m_geometry.width(), m_geometry.height());
}

/****************************************************************************************/
/*								 Waterfall Plotting Functions							*/
/****************************************************************************************/

void QGLchart::initializeBoxWaterFallProgram()
{
    m_boxWaterFallProgram = new QOpenGLShaderProgram();
    m_boxWaterFallProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/boxWaterFall.vert");
    m_boxWaterFallProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/boxWaterFall.frag");
    m_boxWaterFallProgram->link();
    m_boxWaterFallProgram->bind();

    // Cache Uniform Locations
    u_boxWaterFallModelToWorld = m_boxWaterFallProgram->uniformLocation("modelToWorld");
    u_boxWaterFallWorldToCamera = m_boxWaterFallProgram->uniformLocation("worldToCamera");
    u_boxWaterFallCameraToView = m_boxWaterFallProgram->uniformLocation("cameraToView");

    //Create Vertex Array Object
    m_boxWaterFallObject.create();
    m_boxWaterFallObject.bind();

    m_boxWaterFallBuffer.create();
    m_boxWaterFallBuffer.bind();
    m_boxWaterFallBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_boxWaterFallBuffer.allocate(m_boxWaterFallPosition.constData(), m_boxWaterFallPosition.count() * sizeof(GLfloat) * 4);

    m_boxWaterFallProgram->enableAttributeArray(0);
    m_boxWaterFallBuffer.bind();
    m_boxWaterFallProgram->setAttributeBuffer(0, GL_FLOAT, 0, 4, 0);

    // Release (unbind) all
    m_boxWaterFallBuffer.release();
    m_boxWaterFallObject.release();

    m_boxWaterFallProgram->release();
}

void QGLchart::updateOldBoxWaterFallProgram()
{
    /*auto t1 = m_newWaterfallFrameBuffer->size().width();
    auto t2 = m_newWaterfallFrameBuffer->size().height();*/
    if (m_chartType == Waterfall)
    {
        f->glViewport(0, -7, m_newWaterfallFrameBuffer->size().width(), m_newWaterfallFrameBuffer->size().height());
    }
    else if (m_chartType == HorizantalWaterfall)
    {
        f->glViewport(6, 0, m_newWaterfallFrameBuffer->size().width(), m_newWaterfallFrameBuffer->size().height());
    }

    //m_newWaterfallFrameBuffer->bind();
    f->glEnable(GL_BLEND);
    f->glEnable(GL_TEXTURE_2D);

    f->glBlendFunc(GL_ONE, GL_ZERO);
    f->glBlendEquation(GL_FUNC_ADD);

    m_projection.setToIdentity();

    m_projection.ortho(-1.0f, 1.0f, -1.0f, 1.0f, -20.0f, 20.0f);

    m_boxWaterFallProgram->bind();
    m_boxWaterFallObject.bind();
    f->glActiveTexture(GL_TEXTURE0);
    m_oldWaterfallFrameBuffer->texture();
    f->glBindTexture(GL_TEXTURE_2D, m_oldWaterfallFrameBuffer->texture());
    m_boxWaterFallProgram->setUniformValue("texture0", 0);

    m_boxWaterFallProgram->setUniformValue(u_boxWaterFallWorldToCamera, m_camera.toMatrix());
    m_boxWaterFallProgram->setUniformValue(u_boxWaterFallCameraToView, m_projection);
    m_boxWaterFallProgram->setUniformValue(u_boxWaterFallModelToWorld, m_transform.toMatrix());

    //m_boxWaterFallBuffer.allocate(m_boxWaterFallPosition.constData(), m_boxWaterFallPosition.count() * sizeof(GLfloat)* 2);

    //m_boxWaterFallProgram->enableAttributeArray(0);
    //m_boxWaterFallBuffer.bind();
    //m_boxWaterFallProgram->setAttributeBuffer(0, GL_FLOAT, 0, 4, 0);

    f->glDrawArrays(GL_TRIANGLES, 0, m_boxWaterFallPosition.count());

    m_boxWaterFallObject.release();
    m_newWaterfallFrameBuffer->release();
    m_boxWaterFallProgram->release();
    f->glBindTexture(GL_TEXTURE_2D, 0);
    f->glDisable(GL_TEXTURE_2D);
    f->glDisable(GL_BLEND);
    //m_newWaterfallFrameBuffer->release();

}

void QGLchart::updateBoxWaterFallProgram()
{
    f->glEnable(GL_BLEND);
    f->glEnable(GL_TEXTURE_2D);

    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glBlendEquation(GL_FUNC_ADD);
    f->glViewport(m_viewPortBottomLeft.x(), m_viewPortBottomLeft.y(), m_viewPortSize.first, m_viewPortSize.second);
    m_projection.setToIdentity();

    m_projection.ortho(-1.0f, 1.0f, -1.0f, 1.0f, -20.0f, 20.0f);

    m_boxWaterFallProgram->bind();
    m_boxWaterFallObject.bind();
    f->glActiveTexture(GL_TEXTURE0);
    m_newWaterfallFrameBuffer->texture();
    f->glBindTexture(GL_TEXTURE_2D, m_newWaterfallFrameBuffer->texture());
    m_boxWaterFallProgram->setUniformValue("texture0", 0);

    m_boxWaterFallProgram->setUniformValue(u_boxWaterFallWorldToCamera, m_camera.toMatrix());
    m_boxWaterFallProgram->setUniformValue(u_boxWaterFallCameraToView, m_projection);
    m_boxWaterFallProgram->setUniformValue(u_boxWaterFallModelToWorld, m_transform.toMatrix());


    f->glDrawArrays(GL_TRIANGLES, 0, m_boxWaterFallPosition.count());

    m_boxWaterFallObject.release();
    m_boxWaterFallProgram->release();
    f->glBindTexture(GL_TEXTURE_2D, 0);
    f->glDisable(GL_TEXTURE_2D);
    f->glDisable(GL_BLEND);
}

void QGLchart::initializeColorPalettesProgram()
{
    //Create Shader (DO not release unit VAO is created)
    m_colorPalettesProgram = new QOpenGLShaderProgram();
    m_colorPalettesProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/drawColorPalettes.vert");
    m_colorPalettesProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/drawColorPalettes.frag");
    m_colorPalettesProgram->link();
    m_colorPalettesProgram->bind();

    // Cache Uniform Locations
    u_colorPalettesModelToWorld = m_colorPalettesProgram->uniformLocation("modelToWorld");
    u_colorPalettesWorldToCamera = m_colorPalettesProgram->uniformLocation("worldToCamera");
    u_colorPalettesCameraToView = m_colorPalettesProgram->uniformLocation("cameraToView");

    //Create Buffer (Do not release until VAO is created)


    //Create Vertex Array Object
    m_colorPalettesObject.create();
    m_colorPalettesObject.bind();

    m_colorPalettesBuffer.create();
    m_colorPalettesBuffer.bind();
    m_colorPalettesBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_colorPalettesBuffer.allocate(m_colorPalettesPosition.constData(), m_colorPalettesPosition.count() * sizeof(GLfloat) * 2);

    m_colorPalettesProgram->enableAttributeArray(0);
    m_colorPalettesBuffer.bind();
    m_colorPalettesProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

    // Release (unbind) all
    m_colorPalettesBuffer.release();
    m_colorPalettesObject.release();
    m_colorPalettesProgram->release();
}

void QGLchart::updateColorPalettesProgram()
{
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glBlendEquation(GL_FUNC_ADD);
    f->glViewport((int)(m_geometry.width() * 0.01), m_viewPortBottomLeft.y(), m_viewPortSize.first * 0.020, m_viewPortSize.second);

    m_projection.setToIdentity();

    m_projection.ortho(-1.0f, 1.0f /*4095*/, 0.0f /** 2.0f*/, 1.0f/* * 2.0f*/, -20.0f, 20.0f);

    m_colorPalettesProgram->bind();
    m_colorPalettesObject.bind();

    m_colorPalettesBuffer.bind();

    m_colorPalettesProgram->setUniformValue(u_colorPalettesWorldToCamera, m_camera.toMatrix());
    m_colorPalettesProgram->setUniformValue(u_colorPalettesCameraToView, m_projection);
    m_colorPalettesProgram->setUniformValue(u_colorPalettesModelToWorld, m_transform.toMatrix());

    f->glDrawArrays(GL_TRIANGLE_STRIP, 0, m_colorPalettesPosition.count());

    m_colorPalettesObject.release();
    m_colorPalettesBuffer.release();
    m_colorPalettesProgram->release();
    f->glDisable(GL_BLEND);
}

void QGLchart::initializeScatterPlotProgram()
{
    m_scatterPlotProgram = new QOpenGLShaderProgram();
    m_scatterPlotProgram->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resources/shaders/target.vert");
    m_scatterPlotProgram->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resources/shaders/target.frag");
    m_scatterPlotProgram->link();
    m_scatterPlotProgram->bind();

    u_scatterPlotModelToWorld	=	m_scatterPlotProgram->uniformLocation("modelToWorld");
    u_scatterPlotWorldToCamera	=	m_scatterPlotProgram->uniformLocation("worldToCamera");
    u_scatterPlotCameraToView	=	m_scatterPlotProgram->uniformLocation("cameraToView");
    u_scatterColor				=	m_scatterPlotProgram->uniformLocation("color");
    u_scatterPlotStretch		=	m_scatterPlotProgram->uniformLocation("stretch");
    u_xScatterPlotScale			=	m_scatterPlotProgram->uniformLocation("xscale");
    u_yScatterPlotScale			=	m_scatterPlotProgram->uniformLocation("yscale");

    m_scatterPlotObject.create();
    m_scatterPlotObject.bind();

    m_scatterBoxBuffer.create();
    m_scatterBoxBuffer.bind();
    m_scatterBoxBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    m_scatterBoxBuffer.allocate(m_scatterBoxPosition.constData(), m_scatterBoxPosition.count() * sizeof(GLfloat) * 2);

    m_scatterCenterBuffer.create();
    m_scatterCenterBuffer.bind();
    m_scatterCenterBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_scatterCenterBuffer.allocate(NULL, 10000 * sizeof(GLfloat) * 2);

    m_scatterPlotProgram->enableAttributeArray(0);
    m_scatterBoxBuffer.bind();
    m_scatterPlotProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

    m_scatterPlotProgram->enableAttributeArray(1);
    m_scatterCenterBuffer.bind();
    m_scatterPlotProgram->setAttributeBuffer(1, GL_FLOAT, 0, 2, 0);

    m_scatterCenterBuffer.release();
    m_scatterBoxBuffer.release();
    m_scatterPlotObject.release();
    m_scatterPlotProgram->release();
}

void QGLchart::updateScatterPlotProgram(QCustomSeries* series)
{
    f->glEnable(GL_BLEND);
    f->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    f->glBlendEquation(GL_FUNC_ADD);

    f->glViewport(m_viewPortBottomLeft.x(), m_viewPortBottomLeft.y(), m_viewPortSize.first, m_viewPortSize.second);
    m_projection.setToIdentity();

    if ((m_zoomAreaSelected) && (!m_SyncedZoom))
    {
        m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
    }
    else
    {
        m_ChartHorizantalRange = getChartHorizantalRange();
        m_ChartVerticalRange = getChartVerticalRange();

        QVector2D rangeX = m_ChartHorizantalRange;
        QVector2D rangeY = m_ChartVerticalRange;

        m_zoomedData.setX(rangeX.x());
        m_zoomedData.setY(rangeX.y());
        m_zoomedData.setZ(rangeY.x());
        m_zoomedData.setW(rangeY.y());

        m_projection.ortho(m_zoomedData.x(), m_zoomedData.y(), m_zoomedData.z(), m_zoomedData.w(), -20.0f, 20.0f);
        //m_projection.ortho(rangeX.x(), rangeX.y(), rangeY.x(), rangeY.y(), -20.0f, 20.0f);
    }

    m_scatterPlotProgram->bind();
    m_scatterPlotObject.bind();

    m_scatterPlotProgram->setUniformValue(u_scatterPlotWorldToCamera, m_camera.toMatrix());
    m_scatterPlotProgram->setUniformValue(u_scatterPlotCameraToView, m_projection);
    m_scatterPlotProgram->setUniformValue(u_scatterPlotModelToWorld, m_transform.toMatrix());
    m_scatterPlotProgram->setUniformValue(u_scatterColor, series->seriesColor());

    m_scatterCenterBuffer.bind();
    m_scatterCenterBuffer.allocate(series->pointsVector().constData(), series->seriesSize() * sizeof(GLfloat) * 2);

    m_scatterPlotProgram->setUniformValue(u_xScatterPlotScale, calculateScatterScale(m_viewPortSize.first, (m_zoomedData.y() - m_zoomedData.x())));
    m_scatterPlotProgram->setUniformValue(u_yScatterPlotScale, calculateScatterScale(m_viewPortSize.second, (m_zoomedData.w() - m_zoomedData.z())));
    m_scatterPlotProgram->enableAttributeArray(0);
    m_scatterBoxBuffer.bind();
    m_scatterPlotProgram->setAttributeBuffer(0, GL_FLOAT, 0, 2, 0);

    m_scatterPlotProgram->enableAttributeArray(1);
    m_scatterCenterBuffer.bind();
    m_scatterCenterBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    m_scatterPlotProgram->setAttributeBuffer(1, GL_FLOAT, 0, 2, 0);

    fExt->glVertexAttribDivisor(0, 0);
    fExt->glVertexAttribDivisor(1, 1);

    fExt->glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, series->seriesSize());

    m_scatterPlotObject.release();
    m_scatterCenterBuffer.release();
    m_scatterPlotProgram->release();
    f->glDisable(GL_BLEND);
    f->glViewport(0, 0, m_geometry.width(), m_geometry.height());
}

float QGLchart::calculateScatterScale(int windowLength, float axisRange)
{
    float axisScale;
    axisScale = 2 * (axisRange / windowLength);
    return axisScale;
}

void QGLchart::rectToVector(const QRectF &rect, QVector<QVector3D> *vector)
{
    vector->append(QVector3D(rect.bottomLeft()));
    vector->append(QVector3D(rect.bottomRight()));
    vector->append(QVector3D(rect.topLeft()));
    vector->append(QVector3D(rect.topRight()));
}

void QGLchart::calculateViewPort(const QRect &geometry, const QMarginsF &margins)
{
    m_viewPortBottomLeft.setX((int)( ((qreal)geometry.width()) * margins.left()));
    m_viewPortBottomLeft.setY((int)( ((qreal)geometry.height()) * margins.bottom()));

    m_viewPortSize.first = (int)(((qreal)geometry.width()) * (1 - margins.left() - margins.right()));
    m_viewPortSize.second = (int)(((qreal)geometry.height()) * (1 - margins.top() - margins.bottom()));

    QPoint viewportTopLeft;
    viewportTopLeft.setX((int)(((qreal)geometry.width()) * margins.left()));
    viewportTopLeft.setY((int)(((qreal)geometry.height()) * margins.top()));

    QRect temp;
    temp.setWidth(m_viewPortSize.first);
    temp.setHeight(m_viewPortSize.second);
    temp.moveTopLeft(viewportTopLeft);
    m_drawRegion = temp;
}

int QGLchart::maxSeriesSize()
{
    int maxSeriesSizeHolder = 0;
    for (int i = 0; i < m_addedSeriesData.count(); i++)
    {
        if ((m_addedSeriesData.at(i).isVisible()) && (m_addedSeriesData.at(i).seriesSize() > maxSeriesSizeHolder))
            maxSeriesSizeHolder = m_addedSeriesData.at(i).seriesSize();
    }

    return maxSeriesSizeHolder;
}

QVector3D QGLchart::qColorToQVector3D(const QColor &color)
{
    QVector3D colorVector;
    colorVector.setX(color.red());
    colorVector.setY(color.green());
    colorVector.setZ(color.blue());
    return colorVector;
}
