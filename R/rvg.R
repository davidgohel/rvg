#' @useDynLib rvg
#' @title Create SVG Graphic
#' @param rootname root filename to create. For each plot, a file is produced. 
#' The name of the file is the concatenation of \code{rootname}, \code{plot_id} 
#' plot index (it will be incremented for each new plot) and the extention \code{.svg}.
#' @param width width in pixel
#' @param height height in pixel
#' @param ps point size
#' @param fontname font name
#' @param plot_id starting plot index. Default to 1.
#' @examples 
#' rvg("myplot")
#' with( iris, plot( Sepal.Length, Petal.Length ) )
#' dev.off()
#' 
#' # an interactive example
#' rvg("myiplot")
#'     with( iris, plot( Sepal.Length, Petal.Length , type = "n" ) )
#'    
#'    sdata = split( iris, iris$Species )
#'    
#'    for(i in names( sdata ) ){
#'      tempdata = sdata[[i]]
#'      rvg_tracer_on()
#'      points(x = tempdata$Sepal.Length , y = tempdata$Petal.Length, pch = 16 )
#'      ids = rvg_tracer_off()
#'  
#'      send_tooltip(as.integer(ids), as.character(tempdata$Petal.Length) )
#'      send_click(as.integer(ids), paste0("function(){alert('", tempdata$Species,"')}") )
#'     }
#' dev.off()
#' @export 
rvg = function( rootname, width = 504, height = 504, 
		ps = 12, fontname = "Helvetica", 
		plot_id = 1 ) {

	.Call("R_rvg_Device", rootname, as.double(width), as.double(height), 
			as.double(ps), fontname, 
			as.integer( plot_id - 1 )
	)
	
	invisible()
}

