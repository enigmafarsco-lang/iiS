#ifndef CONFIG
#define CONFIG


#ifndef PREFIX
#	define PREFIX "/usr/local"
#endif

#define OSC_GLADE_FILE_PATH PREFIX "/share/osc/"
#define OSC_PLUGIN_PATH PREFIX "/lib/osc/"
#define OSC_XML_PATH PREFIX "/lib/osc/xmls"
#define OSC_FILTER_FILE_PATH PREFIX "/lib/osc/filters"
#define OSC_WAVEFORM_FILE_PATH PREFIX "/lib/osc/waveforms"
#define OSC_PROFILES_FILE_PATH PREFIX "/lib/osc/profiles"

#define TIME_DOMAIN "timeDomain"
#define SMART_TIME_DOMAIN "smartTimeDomain"
#define FRQ_DOMAIN "frqDomain"
#define FRQ_SEEK "frqSeek"

#endif // CONFIG

