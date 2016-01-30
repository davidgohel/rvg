#' @import grid
#' @title Generate interactive grob points
#' @description This function can be used to generate interactive grob
#' points.
#' @inheritParams grid::pointsGrob
#' @param tooltip tooltip associated with points
#' @param onclick javascript action to execute when point is clicked
#' @param data_id identifiers to associate with points
#' @param use_jquery logical indicating whether to rely on jQuery or not (default: "\code{TRUE}")
#' @export
interactivePointsGrob <- function( x = unit(0.5, "npc"),
		y = unit(0.5, "npc"),
		tooltip = NULL,
		onclick = NULL,
		data_id = NULL,
		use_jquery = TRUE,
		pch=1, size=unit(1, "char"),
		default.units="native",
		name=NULL, gp=gpar(), vp=NULL) {
	if (!is.unit(x))
		x <- unit(x, default.units)
	if (!is.unit(y))
		y <- unit(y, default.units)
	grob(tooltip = tooltip, onclick = onclick, data_id = data_id, use_jquery = use_jquery,
			x = x, y = y, pch = pch, size=size,
			name=name, gp=gp, vp=vp, cl="interactivePointsGrob")
}

#' @export
#' @title interactivePointsGrob drawing
#' @description draw an interactivePointsGrob
#' @inheritParams grid::drawDetails
drawDetails.interactivePointsGrob <- function(x, recording) {
	rvg_tracer_on()
	grid.points(x = x$x, y = x$y, pch = x$pch, size = x$size, default.units = "native", gp = x$gp )
	ids = rvg_tracer_off()

  use_jquery <- x$use_jquery %||% TRUE

	if( length( ids ) > 0 ) {
		if( !is.null( x$tooltip ))
			send_tooltip(ids, x$tooltip, use_jquery)
		if( !is.null( x$onclick ))
			send_click(ids, x$onclic, use_jquery)
		if( !is.null( x$data_id ))
			set_data_id(ids, x$data_id, use_jquery)
	}

	invisible()
}

