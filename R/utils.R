#' @import graphics 
#' @import grDevices

#' @title trace on id colection
#'
#' @description Start collecting id of an rvg device. 
#' @export 
rvg_tracer_on <- function(){
	dl <- dev.list()
	if( length( dl ) < 1 )
		stop("cannot find any open graphical device")
	
	if( .Device == "rvg" ) .C("set_tracer_on", (dev.cur()-1L))
	invisible()
}

#' @title trace off id colection
#'
#' @description get collected id of an rvg device and 
#' stop collecting. 
#' @return graphical elements id as integer values.
#' @export 
rvg_tracer_off <- function(){
	dl <- dev.list()
	if( length( dl ) < 1 )
		stop("cannot find any open graphical device")
	if( .Device == "rvg" ){
		ids = .C("collect_id", (dev.cur()-1L), integer(2))[[2]]
		.C("set_tracer_off", (dev.cur()-1L))
	} else ids = integer(0)

	if( length( ids ) < 1 || any( ids < 0 ) )
		return( integer(0) )

	seq(from = ids[1], to = ids[2])
	
}

#' @title send tooltip
#'
#' @description add tooltips on graphical elements.
#' @param ids integer vector of graphical elements identifiers (returned by 
#' \code{\link{rvg_tracer_off}}).
#' @param tooltips tooltip (html is accepted) text.
#' 
#' @export 
send_tooltip = function( ids, tooltips ){
	
	stopifnot( .Device == "rvg" )
	if( is.factor(tooltips) )
		tooltips = as.character( tooltips )
	
	stopifnot( is.character(tooltips) )
	stopifnot( is.numeric(ids) )
	stopifnot( length(ids) == length(tooltips) )
	
	.C("add_tooltip", (dev.cur()-1L), as.integer(ids), 
			tooltips, length(tooltips) )

	invisible()
}

#' @title send click
#'
#' @description add tooltips on graphical elements.
#' @param ids integer vector of graphical elements identifiers (returned by 
#' \code{\link{rvg_tracer_off}}).
#' @param clicks javascript functions to execute on click actions.
#' 
#' @export 
send_click = function( ids, clicks ){
	stopifnot( .Device == "rvg" )
	if( is.factor(clicks) )
		clicks = as.character( clicks )
	
	stopifnot( is.character(clicks) )
	stopifnot( is.numeric(ids) )
	stopifnot( length(ids) == length(clicks) )
	
	.C("add_click_event", (dev.cur()-1L), as.integer(ids), 
			clicks, length(clicks) )
	invisible()
}

#' @title add id
#'
#' @description add id to graphical elements.
#' @param ids integer vector of graphical elements identifiers (returned by 
#' \code{\link{rvg_tracer_off}}).
#' @param data_id user id to be associated with \code{ids}.
#' 
#' @export 
set_data_id = function( ids, data_id ){
	stopifnot( .Device == "rvg" )
	if( is.factor(data_id) )
		data_id = as.character( data_id )
	
	stopifnot( is.character(data_id) )
	stopifnot( is.numeric(ids) )
	stopifnot( length(ids) == length(data_id) )
	
	.C("add_data_id", (dev.cur()-1L), as.integer(ids), 
			data_id, length(data_id) )
	invisible()
}
