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
  dev_num <- as.integer(dev.cur()-1L)
  if( .Device == "devRVG" ) set_tracer_on(dn = dev_num)
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
  if( .Device == "devRVG" ) {
    dev_num <- as.integer(dev.cur()-1L)
    ids <- collect_id(dev_num)
    set_tracer_off(dn = dev_num)
  } else ids = integer(0)

  ids

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

	stopifnot( .Device == "devRVG" )
	if( is.factor(tooltips) )
		tooltips = as.character( tooltips )

	stopifnot( is.character(tooltips) )
	stopifnot( is.numeric(ids) )
	stopifnot( length(ids) == length(tooltips) )
	dev_num <- as.integer(dev.cur()-1L)
	add_tooltip(dn = dev_num, id = as.integer( ids ), str = tooltips )

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
	stopifnot( .Device == "devRVG" )
	if( is.factor(clicks) )
		clicks = as.character( clicks )

	stopifnot( is.character(clicks) )
	stopifnot( is.numeric(ids) )
	stopifnot( length(ids) == length(clicks) )

	dev_num <- as.integer(dev.cur()-1L)
	add_click_event(dn = dev_num, id = as.integer( ids ), str = clicks )

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
	stopifnot( .Device == "devRVG" )
	if( is.factor(data_id) )
		data_id = as.character( data_id )

	stopifnot( is.character(data_id) )
	stopifnot( is.numeric(ids) )
	stopifnot( length(ids) == length(data_id) )

	dev_num <- as.integer(dev.cur()-1L)
	add_data_id(dn = dev_num, id = as.integer( ids ), str = data_id )

	invisible()
}

