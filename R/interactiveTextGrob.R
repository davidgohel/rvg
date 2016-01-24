#' @title Generate interactive grob text
#' @description This function can be used to generate interactive grob
#' text.
#'
#' @inheritParams grid::textGrob
#' @param tooltip tooltip associated with rectangles
#' @param onclick javascript action to execute when rectangle is clicked
#' @param data_id identifiers to associate with rectangles
#' @export
interactiveTextGrob <- function(label, x=unit(0.5, "npc"), y=unit(0.5, "npc"),
                                tooltip = NULL,
                                onclick = NULL,
                                data_id = NULL,
                                just="centre", hjust=NULL, vjust=NULL,
                                rot=0, check.overlap=FALSE,
                                default.units="npc",
                                name=NULL, gp=gpar(), vp=NULL) {
  if (!is.unit(x))
    x <- unit(x, default.units)
  if (!is.unit(y))
    y <- unit(y, default.units)
  grob(tooltip = tooltip, onclick = onclick, data_id = data_id,
       label=label, x=x, y=y, just=just, hjust=hjust, vjust=vjust,
       rot=rot, check.overlap=check.overlap,
       name=name, gp=gp, vp=vp, cl="interactiveTextGrob")
}


#' @export
#' @title interactiveTextGrob drawing
#' @description draw an interactiveTextGrob
#' @inheritParams grid::drawDetails
drawDetails.interactiveTextGrob <- function(x,recording) {
	rvg_tracer_on()
	argnames = setdiff( names(x), c("tooltip", "onclick", "data_id") )
	do.call( grid.text, x[argnames] )

	ids = rvg_tracer_off()
	if( length( ids ) > 0 ) {

		if( !is.null( x$tooltip ))
			send_tooltip(ids, x$tooltip)
		if( !is.null( x$onclick ))
			send_click(ids, x$onclick)
		if( !is.null( x$data_id ))
			set_data_id(ids, x$data_id)
	}
	invisible()
}
