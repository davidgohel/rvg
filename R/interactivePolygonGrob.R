#' @title Generate interactive grob polygons
#' @description This function can be used to generate interactive grob
#' polygons.
#'
#' @inheritParams grid::polygonGrob
#' @param tooltip tooltip associated with polygons
#' @param onclick javascript action to execute when polygon is clicked
#' @param data_id identifiers to associate with polygons
#' @param use_jquery logical indicating whether to rely on jQuery or not (default: "\code{TRUE}")
#' @export
interactivePolygonGrob <- function(x=unit(c(0, 1), "npc"),
		y=unit(c(0, 1), "npc"),
		id=NULL, id.lengths=NULL,
		tooltip = NULL,
		onclick = NULL,
		data_id = NULL,
		use_jquery = TRUE,
		default.units="npc",
		name=NULL, gp=gpar(), vp=NULL) {
	# Allow user to specify unitless vector;  add default units
	if (!is.unit(x))
		x <- unit(x, default.units)
	if (!is.unit(y))
		y <- unit(y, default.units)
	grob(tooltip = tooltip, onclick = onclick, data_id = data_id, use_jquery = use_jquery,
			x=x, y=y, id=id, id.lengths=id.lengths,
			name=name, gp=gp, vp=vp, cl="interactivePolygonGrob")
}

#' @export
#' @title interactivePolygonGrob drawing
#' @description draw an interactivePolygonGrob
#' @inheritParams grid::drawDetails
drawDetails.interactivePolygonGrob <- function(x,recording) {
	rvg_tracer_on()
	argnames = setdiff( names(x), c("tooltip", "onclick", "data_id", "use_jquery") )
	do.call( grid.polygon, x[argnames] )
	ids = rvg_tracer_off()
	if( length( ids ) > 0 ) {

	  if( is.null(x$id) )
	    x$id <- rep( 1, length(x$x) )

	  posid = which(!duplicated(x$id))

	  use_jquery <- x$use_jquery %||% TRUE

		if( !is.null( x$tooltip ))
			send_tooltip(ids, x$tooltip[posid], use_jquery)
		if( !is.null( x$onclick ))
			send_click(ids, x$onclick[posid], use_jquery)
		if( !is.null( x$data_id ))
			set_data_id(ids, x$data_id[posid], use_jquery)

	}


	invisible()
}


