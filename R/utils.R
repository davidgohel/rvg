#' @title trace id on signal
#'
#' @description trace id on signal to rvg device. Internal use only.
#' @export 
rvg_tracer_on <- function(){
	dl <- dev.list()
	if( length( dl ) < 1 )
		stop("cannot find any open graphical device")
	
	if( .Device == "rvg" ) .C("set_tracer_on", (dev.cur()-1L))
	invisible()
}

#' @title trace id off signal
#'
#' @description trace id off signal to rvg device. Internal use only.
#' @export 
rvg_tracer_off <- function(){
	dl <- dev.list()
	if( length( dl ) < 1 )
		stop("cannot find any open graphical device")
	if( .Device == "rvg" ){
		ids = .C("collect_id", (dev.cur()-1L), integer(2))[[2]]
		.C("set_tracer_off", (dev.cur()-1L))
	} else ids = integer(0)
	
	if( length( ids ) < 1 || any( ids ) < 0 )
		return( integer(0) )

	seq(from = ids[1], to = ids[2])
	
}

#' @title send tooltip
#'
#' @description add tooltips on points
#' @param ids integer vector of graphical elements identifiers
#' @param tooltips tooltip (html is accepted) text
#' @export 
sendTooltip = function( ids, tooltips ){
	
	stopifnot( .Device == "rvg" )
	if( is.factor(tooltips) )
		ids = as.character( tooltips )
	
	stopifnot( is.character(tooltips) )
	stopifnot( is.numeric(ids) )
	stopifnot( length(ids) == length(tooltips) )
	
	.C("add_tooltip", (dev.cur()-1L), as.integer(ids), 
			tooltips, length(tooltips) )

	invisible()
}

#' @title send click
#'
#' @export 
sendClick = function( ids, clicks ){
	.C("add_click_event", (dev.cur()-1L), as.integer(ids), 
			as.character( clicks ), length(clicks) )
	invisible()
}
