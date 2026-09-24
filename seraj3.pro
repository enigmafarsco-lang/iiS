QT       += core gui printsupport openglextensions opengl network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11


QMAKE_CXXFLAGS += -liio
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += /usr/include/freetype2

LIBS += -liio -ldl -lfftw3 -lmatio -lfreetype

packagesExist(glib-2.0)
{
    CONFIG += link_pkgconfig
    PKGCONFIG += glib-2.0
}


SOURCES += \
    Communications/appconfiguration.cpp \
    Communications/tcplan.cpp \
    Communications/tcpserver.cpp \
    Communications/udplan.cpp \
    components/GLChart/QCustomAxis.cpp \
    components/GLChart/QCustomSeries.cpp \
    components/GLChart/QGLchart.cpp \
    components/GLChart/SetUpTheme.cpp \
    components/GLChart/UpdateData.cpp \
    components/GLChart/camera3d.cpp \
    components/GLChart/transform3d.cpp \
    components/Pan/pancomponent.cpp \
    components/Pan/pangauge.cpp \
    components/Pan/qmeterdrawfunctions.cpp \
    components/Tilt/qmeterdrawfunctions_tilt.cpp \
    components/Tilt/tiltcomponent.cpp \
    components/Tilt/tiltgauge.cpp \
    components/adrv/ADRV.cpp \
    components/chart/qcustomplot.cpp \
    components/led/led.cpp \
    components/led/widgetwithbackground.cpp \
    devices/cooling/cooling3.cpp \
    devices/cooling/packetmanager.cpp \
    devices/cooling/tcpclient.cpp \
    devices/exciter/exciter.cpp \
    devices/joshan/datastruct.cpp \
    devices/joshan/joshan.cpp \
    devices/rff/rff.cpp \
    devices/sattar/components/Store.cpp \
    devices/sattar/components/fft/fftcalculator.cpp \
    devices/sattar/components/sattarutils.cpp \
    devices/sattar/pages/frqPage.cpp \
    devices/sattar/pages/mainPage.cpp \
    devices/sattar/pages/menupage.cpp \
    devices/sattar/pages/modulationPage.cpp \
    devices/sattar/pages/plotsignal.cpp \
    devices/sattar/pages/priPage.cpp \
    devices/sattar/pages/pulsePage.cpp \
    devices/sattar/pages/pwPage.cpp \
    devices/sattar/pages/radarInfoPage.cpp \
    devices/sattar/pages/spurepage.cpp \
    devices/servo/servo.cpp \
    devices/solid/solid.cpp \
    devices/solid/soliddialog.cpp \
    devices/targetDetector/targetdetector.cpp \
    devices/tuner/tuner.cpp \
    form.cpp \
    main.cpp \
    mainwindow.cpp \
    receiver/calculateplotdata.cpp \
    receiver/clustring/constants.cpp \
    receiver/clustring/dataextractor.cpp \
    receiver/clustring/detector.cpp \
    receiver/clustring/enumerations.cpp \
    receiver/clustring/parameters.cpp \
    receiver/clustring/pulse.cpp \
    receiver/connectdialog.cpp \
    receiver/datatypes.cpp \
    receiver/globals.cpp \
    receiver/iio_utils.cpp \
    receiver/iio_widget.cpp \
    receiver/libini.cpp \
    receiver/libini2.cpp \
    receiver/math_expression_generator.cpp \
    receiver/osc.cpp \
    receiver/oscmain.cpp \
    receiver/oscpreferences.cpp \
    receiver/plot.cpp \
    receiver/plotfunction.cpp \
    receiver/plugins/ad9371.cpp \
    receiver/plugins/ad9371_adv.cpp \
    receiver/plugins/adrv9009.cpp \
    receiver/plugins/adrv9009_adv.cpp \
    receiver/plugins/controlunitadrv9009.cpp \
    receiver/plugins/dacDataManager.cpp \
    receiver/settings.cpp \
    receiver/receivermain.cpp \
    receiver/utils/filedialog.cpp \
    statuswidget.cpp \
    usertype.cpp \
    utils.cpp

HEADERS += \
    Communications/appconfiguration.h \
    Communications/tcplan.h \
    Communications/tcpserver.h \
    Communications/udplan.h \
    components/GLChart/QCustomAxis.h \
    components/GLChart/QCustomSeries.h \
    components/GLChart/QGLchart.h \
    components/GLChart/SetUpTheme.h \
    components/GLChart/UpdateData.h \
    components/GLChart/camera3d.h \
    components/GLChart/newType.h \
    components/GLChart/transform3d.h \
    components/Pan/pancomponent.h \
    components/Pan/pangauge.h \
    components/Tilt/tiltcomponent.h \
    components/Tilt/tiltgauge.h \
    components/adrv/ADRV.h \
    components/chart/qcustomplot.h \
    components/led/led.h \
    components/led/widgetwithbackground.h \
    constants/project_constans.h \
    devices/cooling/cooling3.h \
    devices/cooling/packetmanager.h \
    devices/cooling/tcpclient.h \
    devices/exciter/exciter.h \
    devices/joshan/datastruct.h \
    devices/joshan/joshan.h \
    devices/rff/rff.h \
    devices/sattar/components/Store.h \
    devices/sattar/components/constant.h \
    devices/sattar/components/fft/fftcalculator.h \
    devices/sattar/components/sattarutils.h \
    devices/sattar/pages/frqPage.h \
    devices/sattar/pages/mainPage.h \
    devices/sattar/pages/menupage.h \
    devices/sattar/pages/modulationPage.h \
    devices/sattar/pages/plotsignal.h \
    devices/sattar/pages/priPage.h \
    devices/sattar/pages/pulsePage.h \
    devices/sattar/pages/pwPage.h \
    devices/sattar/pages/radarInfoPage.h \
    devices/sattar/pages/spurepage.h \
    devices/servo/servo.h \
    devices/solid/solid.h \
    devices/solid/soliddialog.h \
    devices/targetDetector/targetdetector.h \
    devices/tuner/tuner.h \
    form.h \
    mainwindow.h \
    receiver/backtrace.h \
    receiver/calculateplotdata.h \
    receiver/clustring/constants.h \
    receiver/clustring/dataextractor.h \
    receiver/clustring/detector.h \
    receiver/clustring/enumerations.h \
    receiver/clustring/parameters.h \
    receiver/clustring/pulse.h \
    receiver/compat.h \
    receiver/config.h \
    receiver/connectdialog.h \
    receiver/custom/tabbar.h \
    receiver/custom/tabwidget.h \
    receiver/datatypes.h \
    receiver/globals.h \
    receiver/iio_utils.h \
    receiver/iio_widget.h \
    receiver/ini.h \
    receiver/libini2.h \
    receiver/math_expression_generator.h \
    receiver/osc.h \
    receiver/osc_plugin.h \
    receiver/oscmain.h \
    receiver/oscpreferences.h \
    receiver/plot.h \
    receiver/plotfunction.h \
    receiver/plugins/ad9371.h \
    receiver/plugins/ad9371_adv.h \
    receiver/plugins/adrv9009.h \
    receiver/plugins/adrv9009_adv.h \
    receiver/plugins/controlunitadrv9009.h \
    receiver/plugins/dacDataManager.h \
    receiver/settings.h \
    receiver/receivermain.h \
    receiver/utils/filedialog.h \
    statuswidget.h \
    usertype.h \
    utils.h

FORMS += \
    devices/cooling/cooling3.ui \
    devices/exciter/exciter.ui \
    devices/joshan/joshan.ui \
    devices/rff/rff.ui \
    devices/sattar/pages/frqPage.ui \
    devices/sattar/pages/mainPage.ui \
    devices/sattar/pages/menupage.ui \
    devices/sattar/pages/modulationPage.ui \
    devices/sattar/pages/plotsignal.ui \
    devices/sattar/pages/priPage.ui \
    devices/sattar/pages/pulsePage.ui \
    devices/sattar/pages/pwPage.ui \
    devices/sattar/pages/radarInfoPage.ui \
    devices/sattar/pages/scanRadar.ui \
    devices/sattar/pages/selectRadar.ui \
    devices/servo/servo.ui \
    devices/solid/solid.ui \
    devices/solid/soliddialog.ui \
    devices/targetDetector/targetdetector.ui \
    devices/tuner/tuner.ui \
    form.ui \
    mainwindow.ui \
    receiver/connectdialog.ui \
    receiver/oscmain.ui \
    receiver/plot.ui \
    receiver/plugins/ad9371.ui \
    receiver/plugins/ad9371_adv.ui \
    receiver/plugins/adrv9009.ui \
    receiver/plugins/adrv9009_adv.ui \
    receiver/plugins/controlunitadrv9009.ui \
    receiver/receivermain.ui \
    receiver/utils/filedialog.ui \
    statuswidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    receiver/README.md \
    receiver/custom-icons/deny.png \
    receiver/custom-icons/fullScreen.png \
    receiver/custom-icons/minus.png \
    receiver/custom-icons/next.png \
    receiver/custom-icons/openFile.ico \
    receiver/custom-icons/play.png \
    receiver/custom-icons/plus.png \
    receiver/custom-icons/plusWindow.png \
    receiver/custom-icons/saveAs.png \
    receiver/custom-icons/select.png \
    receiver/custom-icons/stop.png \
    resources/Images/Icons/Arrow-Down-96.png \
    resources/Images/Icons/Arrow-DownLeft-96.png \
    resources/Images/Icons/Arrow-DownRight-96.png \
    resources/Images/Icons/Arrow-Left-96.png \
    resources/Images/Icons/Arrow-Right-96.png \
    resources/Images/Icons/Arrow-Up-96.png \
    resources/Images/Icons/Arrow-UpLeft-96.png \
    resources/Images/Icons/Arrow-UpRight-96.png \
    resources/Images/Icons/CheckMark-128.png \
    resources/Images/Icons/Compass-icon.png \
    resources/Images/Icons/ComputerLock-256.png \
    resources/Images/Icons/CrossMark-256.png \
    resources/Images/Icons/Exciter-Off-128.png \
    resources/Images/Icons/Exciter-On-128.png \
    resources/Images/Icons/Exit-128.png \
    resources/Images/Icons/History-128.ico \
    resources/Images/Icons/Information-128.png \
    resources/Images/Icons/Led-Green-128.png \
    resources/Images/Icons/Led-Red-128.png \
    resources/Images/Icons/Led-Yellow-128.png \
    resources/Images/Icons/Lock-128.png \
    resources/Images/Icons/Logs-128.png \
    resources/Images/Icons/Park.png \
    resources/Images/Icons/Question-128.png \
    resources/Images/Icons/Rotator-128.png \
    resources/Images/Icons/Scenario-128.ico \
    resources/Images/Icons/Set-Parameters-128.png \
    resources/Images/Icons/Settings-128.png \
    resources/Images/Icons/Sign-Stop-icon.png \
    resources/Images/Icons/StopSign-96.png \
    resources/Images/Icons/Switch-Off-128.png \
    resources/Images/Icons/Switch-On-128.png \
    resources/Images/Icons/Table-128.png \
    resources/Images/Icons/UserInfo-128.png \
    resources/Images/Icons/logo.ico \
    resources/Images/Pictures/ppi.png \
    resources/QSS-master/AMOLED.qss \
    resources/QSS-master/Aqua.qss \
    resources/QSS-master/ConsoleStyle.qss \
    resources/QSS-master/ElegantDark.qss \
    resources/QSS-master/LICENSE \
    resources/QSS-master/ManjaroMix.qss \
    resources/QSS-master/MaterialDark.qss \
    resources/QSS-master/README.md \
    resources/QSS-master/StyleSheet.qss \
    resources/QSS-master/Ubuntu.qss \
    resources/QSS-master/aref.qss \
    resources/QSS-master/aref2.qss \
    resources/docs/jaras10_user_guide.pdf \
    resources/icons/73028-warning-icon.png \
    resources/icons/Marker.png \
    resources/icons/PlayPause.png \
    resources/icons/Warning_2 - Copy.png \
    resources/icons/Warning_2.png \
    resources/icons/averageIcon.png \
    resources/icons/blueStone2.png \
    resources/icons/clearData.png \
    resources/icons/deltaMarker.png \
    resources/icons/equalizer.png \
    resources/icons/exitApp.png \
    resources/icons/generalSetting.png \
    resources/icons/greenStone2.png \
    resources/icons/logo.ico \
    resources/icons/logo.png \
    resources/icons/noiseThreshold.png \
    resources/icons/pdw.png \
    resources/icons/redStone2.png \
    resources/icons/scan.png \
    resources/icons/warning.png \
    resources/icons/warning2.png \
    resources/icons/yellowStone2.png \
    resources/icons/zoom.png \
    resources/icons/zoom1.png \
    resources/qss/Aqua.qss \
    resources/qss/DarkTheme.qss \
    resources/qss/MaterialDark.qss \
    resources/qss/coffee.qss \
    resources/qss/default.qss \
    resources/qss/mystylesheet.qss \
    resources/qss/pagefold.qss \
    resources/qss/stylesheet.qss \
    resources/rc/Hmovetoolbar.png \
    resources/rc/Vmovetoolbar.png \
    resources/rc/add.png \
    resources/rc/branch_closed-on.png \
    resources/rc/branch_closed.png \
    resources/rc/branch_open-on.png \
    resources/rc/branch_open.png \
    resources/rc/checkbox_checked.png \
    resources/rc/checkbox_checked_disabled.png \
    resources/rc/checkbox_checked_focus.png \
    resources/rc/checkbox_indeterminate.png \
    resources/rc/checkbox_indeterminate_disabled.png \
    resources/rc/checkbox_indeterminate_focus.png \
    resources/rc/checkbox_unchecked.png \
    resources/rc/checkbox_unchecked_disabled.png \
    resources/rc/checkbox_unchecked_focus.png \
    resources/rc/close-hover.png \
    resources/rc/close-pressed.png \
    resources/rc/close.png \
    resources/rc/delete-24.png \
    resources/rc/down_arrow.png \
    resources/rc/down_arrow_disabled.png \
    resources/rc/edit.png \
    resources/rc/left_arrow.png \
    resources/rc/left_arrow_disabled.png \
    resources/rc/radio_unchecked.png \
    resources/rc/remove.png \
    resources/rc/right_arrow.png \
    resources/rc/right_arrow_disabled.png \
    resources/rc/transparent.png \
    resources/rc/undock-pressed.png \
    resources/rc/undock.png \
    resources/rc/up_arrow.png \
    resources/rc/up_arrow_disabled.png \
    resources/res/NC.png \
    resources/res/alarm.png \
    resources/res/normal.png \
    resources/res/undef.png \
    resources/shaders/Marker.frag \
    resources/shaders/Marker.vert \
    resources/shaders/box.frag \
    resources/shaders/box.vert \
    resources/shaders/boxWaterFall.frag \
    resources/shaders/boxWaterFall.vert \
    resources/shaders/drawColorPalettes.frag \
    resources/shaders/drawColorPalettes.vert \
    resources/shaders/drawLine.frag \
    resources/shaders/drawLine.geo \
    resources/shaders/drawLine.vert \
    resources/shaders/drawLineColorized.frag \
    resources/shaders/drawLineColorized.geo \
    resources/shaders/drawLineColorized.vert \
    resources/shaders/drawVerticalLineColorized.frag \
    resources/shaders/drawVerticalLineColorized.vert \
    resources/shaders/gridLine.frag \
    resources/shaders/gridLine.vert \
    resources/shaders/masking.frag \
    resources/shaders/masking.vert \
    resources/shaders/new1.qss \
    resources/shaders/target.frag \
    resources/shaders/target.vert \
    resources/shaders/text.frag \
    resources/shaders/text.vert \
    resources/shaders/threshold.frag \
    resources/shaders/threshold.vert \
    resources/shaders/zoomArea.frag \
    resources/shaders/zoomArea.vert \
    resources/styleSheet/new1.qss

RESOURCES += \
    application.qrc

DEFINES += LINUX


#unix:!macx: LIBS += -L$$PWD/../../../../../../usr/local/lib/ -lfreetype

#INCLUDEPATH += $$PWD/../../../../../../usr/local/include/freetype2
#DEPENDPATH += $$PWD/../../../../../../usr/local/include/freetype2

#unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/libfreetype.a




#unix:!macx: LIBS += -L$$PWD/../../../../../../usr/local/lib/ -lfftw3

#INCLUDEPATH += $$PWD/../../../../../../usr/local/include
#DEPENDPATH += $$PWD/../../../../../../usr/local/include

#unix:!macx: PRE_TARGETDEPS += $$PWD/../../../../../../usr/local/lib/libfftw3.a
