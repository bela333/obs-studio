#include <obs-module.h>
#include <obs-source.h>
#include <obs.h>
#include <util/platform.h>
#define PI 3.14159265

struct equirotate_data {
	obs_source_t                   *context;

	gs_effect_t                    *effect;
	gs_eparam_t                    *x_param, *y_param, *z_param;
	gs_eparam_t                    *texture_width, *texture_height;

	float                          x, y, z;
	float                          texwidth, texheight;
};

static const char *equirotate_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("EquiRotateFilter");
}

static void equirotate_update(void *data, obs_data_t *settings)
{
	struct equirotate_data *filter = data;
	double x = obs_data_get_double(settings, "x");
	double y = obs_data_get_double(settings, "y");
	double z = obs_data_get_double(settings, "z");

	filter->x = (float)x/180*PI;
	filter->y = (float)y/180*PI;
	filter->z = (float)z/180*PI;
}

static void equirotate_destroy(void *data)
{
	struct equirotate_data *filter = data;

	if (filter->effect) {
		obs_enter_graphics();
		gs_effect_destroy(filter->effect);
		obs_leave_graphics();
	}

	bfree(data);
}

static void *equirotate_create(obs_data_t *settings, obs_source_t *context)
{
	struct equirotate_data *filter =
		bzalloc(sizeof(struct equirotate_data));
	char *effect_path = obs_module_file("equiRotate.effect");

	filter->context = context;

	obs_enter_graphics();

	filter->effect = gs_effect_create_from_file(effect_path, NULL);
	if (filter->effect) {
		filter->x_param = gs_effect_get_param_by_name(filter->effect, "x");
		filter->y_param = gs_effect_get_param_by_name(filter->effect, "y");
		filter->z_param = gs_effect_get_param_by_name(filter->effect, "z");
		filter->texture_width = gs_effect_get_param_by_name(
			filter->effect, "texture_width");
		filter->texture_height = gs_effect_get_param_by_name(
			filter->effect, "texture_height");
	}

	obs_leave_graphics();

	bfree(effect_path);

	if (!filter->effect) {
		equirotate_destroy(filter);
		return NULL;
	}

	equirotate_update(filter, settings);
	return filter;
}

static void equirotate_render(void *data, gs_effect_t *effect)
{
	struct equirotate_data *filter = data;

	if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
				OBS_ALLOW_DIRECT_RENDERING))
		return;

	filter->texwidth =(float)obs_source_get_width(
			obs_filter_get_target(filter->context));
	filter->texheight = (float)obs_source_get_height(
			obs_filter_get_target(filter->context));

	gs_effect_set_float(filter->x_param, filter->x);
	gs_effect_set_float(filter->y_param, filter->y);
	gs_effect_set_float(filter->z_param, filter->z);
	gs_effect_set_float(filter->texture_width, filter->texwidth);
	gs_effect_set_float(filter->texture_height, filter->texheight);

	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);

	UNUSED_PARAMETER(effect);
}

static obs_properties_t *equirotate_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_float_slider(props, "x","X Position", -180.0f, 180.0f, 0.01f);
	obs_properties_add_float_slider(props, "y","Y Position", -180.0f, 180.0f, 0.01f);
	obs_properties_add_float_slider(props, "z","Z Position", -180.0f, 180.0f, 0.01f);

	UNUSED_PARAMETER(data);
	return props;
}

static void equirotate_defaults(obs_data_t *settings)
{
	obs_data_set_default_double(settings, "x", 0);
	obs_data_set_default_double(settings, "y", 0);
	obs_data_set_default_double(settings, "z", 0);
}

struct obs_source_info equirotate_filter = {
	.id = "equirotate_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = equirotate_getname,
	.create = equirotate_create,
	.destroy = equirotate_destroy,
	.update = equirotate_update,
	.video_render = equirotate_render,
	.get_properties = equirotate_properties,
	.get_defaults = equirotate_defaults
};
