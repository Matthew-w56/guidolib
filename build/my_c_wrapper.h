// 
// Matthew Williams
// May 14th, 2024
// 

#ifdef __cplusplus
extern "C" {
#endif

// Typedefs for things later

class VGDevice;
struct NodeAR;
struct NodeGR;
typedef struct NodeAR* ARHandler;
typedef struct NodeGR* GRHandler;
typedef const struct NodeAR* CARHandler;
typedef const struct NodeGR* CGRHandler;
struct GuidoInitDesc
{
	//! a graphic device pointer, if null a default device is used
	VGDevice	*		graphicDevice;

    void *              reserved;

	//! the music font name, defaults to "guido" font when null
	const char *		musicFont;
	//! a text font name, defaults to "times" font when null
	const char *		textFont;
};
struct GPaintStruct
{
    //! a flag to ignore the following rect and to redraw everything
    bool	erase;
    //! Absolute Guido virtual coordinates of the clipping rectangle.
    //! Only systems that intersect with this rectangle will be drawn.
    int		left;
	int		top;
	int		right;
	int		bottom;
};
typedef struct
{
	//! the date numerator
    int num;
	//! the date denominator
    int denom;

} GuidoDate;
struct GuidoOnDrawDesc
{
	//! A Guido handler to a graphic representation
    GRHandler handle;

    //! A graphic device context.
    VGDevice * hdc;
	//! The page number. Starts from 1.
    int page;
    //! Indicates what to (re)draw.
	GPaintStruct updateRegion;

	/*! Indicates the coordinates of the score point that will appear at the graphic
        origin. Typical values are 0. Non null values have the effect of moving a window
        over the score page, like scroll bars that move a page view.
        Units are internal units.
    */
	int scrollx, scrolly;

	/*! Indicates the size of the drawing zone. The size is expressed
        in graphic device units (pixels for a screen for example)
    */
    float reserved;

    int sizex, sizey;

	//! If true, the engine ignores scroll, zoom and sizes parameters.
    //! If false, the engine draws a white background in the graphic device.
    int isprint;
};
enum GuidoErrCode
{
	//! null is used to denote no error
    guidoNoErr					= 0,
	//! error while parsing the Guido format
	guidoErrParse				= -1,
	//! memory allocation error
	guidoErrMemory				= -2,
	//! error while reading or writing a file
	guidoErrFileAccess			= -3,
	//! the user cancelled the action
	guidoErrUserCancel			= -4,
	//! the music font is not available
	guidoErrNoMusicFont			= -5,
	//! the text font is not available
	guidoErrNoTextFont			= -6,
	//! bad parameter used as argument
	guidoErrBadParameter		= -7,
	//! invalid handler used
	guidoErrInvalidHandle		= -8,
	//! required initialisation has not been performed
	guidoErrNotInitialized		= -9,
	//! the action failed
	guidoErrActionFailed		= -10
};

GuidoErrCode GuidoInit(GuidoInitDesc* desc);
GuidoErrCode GuidoInitWithIndependentSVG();
void GuidoShutdown();
GuidoErrCode GuidoParseFile(const char * filename, ARHandler* ar);
GuidoErrCode GuidoParseString(const char * str, ARHandler* ar);

#ifdef __cplusplus
}
#endif
