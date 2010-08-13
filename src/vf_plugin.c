
/**
 * @file
 * plugin
 * Extension written by bilalK odesk>
 */

#include "avfilter.h"
#include "libavutil/common.h"
#include "libavutil/mem.h"
#include "libavutil/pixdesc.h"

#include <dlfcn.h>

typedef struct {
	void* handle;
	int (*init)( const char *args );
	int (*uninit)();
	int (*process)( uint8_t *dst[4], int dst_stride[4],
                        uint8_t *src[4], int src_stride[4],
                        int width, int height );
} PluginContext;

static av_cold int init(AVFilterContext *ctx, const char *args, void *opaque)
{
	char params[256];
	char* pos;
	
	PluginContext *plugin = ctx->priv;
	
	if ( ! args ) {
		av_log(ctx, AV_LOG_ERROR, "no plugin specified\n");
		return -1;
	}

	strncpy( params, args, 255 );
	
	pos = strchr( params, ':' );
	if ( pos == NULL ) {
		av_log(ctx, AV_LOG_ERROR,
		       "invalid input parameter: %s\n", args);
		return -1;
	}
	*pos++ = 0;
	
	av_log(ctx, AV_LOG_DEBUG, "plugin: %s\n", params  );
    
	av_log(ctx, AV_LOG_DEBUG, "params: %s\n", pos );
    
	plugin->handle = dlopen( params, RTLD_NOW );

	if ( ! plugin->handle ) {
		av_log(ctx, AV_LOG_ERROR, "%s: %s\n", params, dlerror());
		return -1;
	}
    
	dlerror();

	*(void **) (&plugin->init) = dlsym(plugin->handle, "init");
	if ( dlerror() != NULL ) {
		av_log(ctx, AV_LOG_ERROR, "cannot find function init\n" );
		return -1;
	}
	*(void **) (&plugin->uninit)  = dlsym(plugin->handle, "uninit");
	if ( dlerror() != NULL ) {
		av_log(ctx, AV_LOG_ERROR, "cannot find function uninit\n" );
		return -1;
	}
	*(void **) (&plugin->process) = dlsym(plugin->handle, "process");
	if ( dlerror() != NULL ) {
		av_log(ctx, AV_LOG_ERROR, "cannot find function process\n" );
		return -1;
	}

	return plugin->init( pos );
}

static int query_formats(AVFilterContext *ctx)
{
	enum PixelFormat pix_fmts[] = {
	    PIX_FMT_RGB24, PIX_FMT_NONE
	};
    
	avfilter_set_common_formats(ctx, avfilter_make_format_list(pix_fmts));

	return 0;
}

static int config_props(AVFilterLink *link)
{
	return 0;
}

static av_cold void uninit(AVFilterContext *ctx)
{
	PluginContext *plugin = ctx->priv;

	if ( plugin->handle ) {
		if ( plugin->uninit() < 0 )
			av_log(ctx, AV_LOG_ERROR, "function uninit failed\n" );
    
		dlclose( plugin->handle );
	}
}

static void end_frame(AVFilterLink *link)
{
	PluginContext *plugin = link->dst->priv;
	AVFilterBufferRef *in  = link->cur_buf;
	AVFilterBufferRef *out = link->dst->outputs[0]->out_buf;
	
	if ( plugin->process( out->data, out->linesize, in->data,
			      in->linesize, link->w, link->h ) < 0 )
		av_log(link->dst, AV_LOG_ERROR, "function process failed\n" );
    
	avfilter_unref_buffer(in);
	avfilter_draw_slice(link->dst->outputs[0], 0, link->h, 1);
	avfilter_end_frame(link->dst->outputs[0]);
	avfilter_unref_buffer(out);
}

static void draw_slice(AVFilterLink *link, int y, int h, int slice_dir)
{
}

AVFilter avfilter_vf_plugin = {
    .name      = "plugin",
    .description = NULL_IF_CONFIG_SMALL("simple plugin framework"),

    .priv_size = sizeof(PluginContext),

    .init = init,
    .uninit = uninit,
    .query_formats = query_formats,

    .inputs    = (AVFilterPad[]) {{ .name             = "default",
                                    .type             = AVMEDIA_TYPE_VIDEO,
                                    .draw_slice       = draw_slice,
                                    .end_frame        = end_frame,
                                    .config_props     = config_props,
                                    .min_perms        = AV_PERM_READ, },
                                  { .name = NULL}},

    .outputs   = (AVFilterPad[]) {{ .name             = "default",
                                    .type             = AVMEDIA_TYPE_VIDEO, },
                                  { .name = NULL}},
};
