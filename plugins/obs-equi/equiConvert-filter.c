#include <obs-module.h>
#include <obs-source.h>
#include <obs.h>
#include <util/platform.h>

struct equiconvert_data {
	obs_source_t                   *context;

	gs_effect_t                    *effect;
	gs_eparam_t                    *width_param, *height_param, *x_param, *y_param;
	gs_eparam_t                    *texture_width, *texture_height;

	float                          width, height, x, y;
	float                          texwidth, texheight;
};

static const char *equiconvert_getname(void *unused)
{
	UNUSED_PARAMETER(unused);
	return obs_module_text("EquiConvertFilter");
}

static void equiconvert_update(void *data, obs_data_t *settings)
{

	struct equiconvert_data *filter = data;
	double width = obs_data_get_double(settings, "width");
	double height = obs_data_get_double(settings, "height");
	double x = obs_data_get_double(settings, "x");
	double y = obs_data_get_double(settings, "y");

	filter->width = (float)width;
	filter->height = (float)height;
	filter->x = (float)x;
	filter->y = (float)y;
}

static void equiconvert_destroy(void *data)
{
	struct equiconvert_data *filter = data;

	if (filter->effect) {
		obs_enter_graphics();
		gs_effect_destroy(filter->effect);
		obs_leave_graphics();
	}

	bfree(data);
}

static void *equiconvert_create(obs_data_t *settings, obs_source_t *context)
{
	struct equiconvert_data *filter =
		bzalloc(sizeof(struct equiconvert_data));
	char *effect_path = obs_module_file("equiConvert.effect");

	filter->context = context;

	obs_enter_graphics();

	filter->effect = gs_effect_create_from_file(effect_path, NULL);
	if (filter->effect) {
		filter->width_param = gs_effect_get_param_by_name(filter->effect, "width");
		filter->height_param = gs_effect_get_param_by_name(filter->effect, "height");
		filter->x_param = gs_effect_get_param_by_name(filter->effect, "x");
		filter->y_param = gs_effect_get_param_by_name(filter->effect, "y");
		filter->texture_width = gs_effect_get_param_by_name(
			filter->effect, "texture_width");
		filter->texture_height = gs_effect_get_param_by_name(
			filter->effect, "texture_height");
	}

	obs_leave_graphics();

	bfree(effect_path);

	if (!filter->effect) {
		equiconvert_destroy(filter);
		return NULL;
	}

	equiconvert_update(filter, settings);
	return filter;
}

static void equiconvert_render(void *data, gs_effect_t *effect)
{
	struct equiconvert_data *filter = data;

	if (!obs_source_process_filter_begin(filter->context, GS_RGBA,
				OBS_ALLOW_DIRECT_RENDERING))
		return;

	filter->texwidth =(float)obs_source_get_width(
			obs_filter_get_target(filter->context));
	filter->texheight = (float)obs_source_get_height(
			obs_filter_get_target(filter->context));

	gs_effect_set_float(filter->width_param, filter->width);
	gs_effect_set_float(filter->height_param, filter->height);
	gs_effect_set_float(filter->x_param, filter->x);
	gs_effect_set_float(filter->y_param, filter->y);
	gs_effect_set_float(filter->texture_width, filter->texwidth);
	gs_effect_set_float(filter->texture_height, filter->texheight);

	obs_source_process_filter_end(filter->context, filter->effect, 0, 0);

	UNUSED_PARAMETER(effect);
}

static obs_properties_t *equiconvert_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();

	obs_properties_add_float_slider(props, "width","Width", 0.0f, 100.0f, 0.01f);
	obs_properties_add_float_slider(props, "height","Height", 0.0f, 100.0f, 0.01f);
	obs_properties_add_float_slider(props, "x","X Position", -100.0f, 100.0f, 0.01f);
	obs_properties_add_float_slider(props, "y","Y Position", -100.0f, 100.0f, 0.01f);

	UNUSED_PARAMETER(data);
	return props;
}

static void equiconvert_defaults(obs_data_t *settings)
{
	obs_data_set_default_double(settings, "width", 1);
	obs_data_set_default_double(settings, "height", 1);
	obs_data_set_default_double(settings, "x", 0);
	obs_data_set_default_double(settings, "y", 0);
}

struct obs_source_info equiconvert_filter = {
	.id = "equiconvert_filter",
	.type = OBS_SOURCE_TYPE_FILTER,
	.output_flags = OBS_SOURCE_VIDEO,
	.get_name = equiconvert_getname,
	.create = equiconvert_create,
	.destroy = equiconvert_destroy,
	.update = equiconvert_update,
	.video_render = equiconvert_render,
	.get_properties = equiconvert_properties,
	.get_defaults = equiconvert_defaults
};
