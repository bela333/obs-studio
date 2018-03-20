#include <obs-module.h>
#include "obs-filters-config.h"

OBS_DECLARE_MODULE()

OBS_MODULE_USE_DEFAULT_LOCALE("obs-equi", "en-US")

extern struct obs_source_info equirotate_filter;
extern struct obs_source_info equiconvert_filter;

bool obs_module_load(void)
{
	obs_register_source(&equirotate_filter);
	obs_register_source(&equiconvert_filter);
	return true;
}
