#' @useDynLib rvg
#' @title Create SVG Graphic
#' @param file filename to create
#' @param width width in pixel
#' @param height height in pixel
#' @param ps point size
#' @param fontname font name
#' @param canvas_id canvas id
#' @export 
rvg = function( file, width = 504, height = 504, 
		ps = 12, fontname = "Helvetica", 
		canvas_id = 0 ) {

	.Call("R_rvg_Device", file, as.double(width), as.double(height), 
			as.double(ps), fontname, 
			as.integer( canvas_id )
	)
	
	invisible()
}

