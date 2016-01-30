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
  if( .Device == "dsvg_device" ) set_tracer_on(dn = dev_num)
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
  if( .Device == "dsvg_device" ) {
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
#' @param use_jquery logical indicating whether to rely on jQuery or not (default: "\code{TRUE}")
#'
#' @export
send_tooltip = function( ids, tooltips, use_jquery=TRUE){

	stopifnot( .Device == "dsvg_device" )
	if( is.factor(tooltips) )
		tooltips = as.character( tooltips )

	stopifnot( is.character(tooltips) )
	stopifnot( is.numeric(ids) )
	stopifnot( length(ids) == length(tooltips) )
	dev_num <- as.integer(dev.cur()-1L)
	if (use_jquery[1] == TRUE) {
  	add_tooltip(dn = dev_num, id = as.integer( ids ), str = tooltips )
	} else {
  	add_tooltip_raw(dn = dev_num, id = as.integer( ids ), str = tooltips )
	}

	invisible()
}

#' @title send click
#'
#' @description add tooltips on graphical elements.
#' @param ids integer vector of graphical elements identifiers (returned by
#' \code{\link{rvg_tracer_off}}).
#' @param clicks javascript functions to execute on click actions.
#' @param use_jquery logical indicating whether to rely on jQuery or not (default: "\code{TRUE}")
#'
#' @export
send_click = function( ids, clicks, use_jquery=TRUE){
	stopifnot( .Device == "dsvg_device" )
	if( is.factor(clicks) )
		clicks = as.character( clicks )

	stopifnot( is.character(clicks) )
	stopifnot( is.numeric(ids) )
	stopifnot( length(ids) == length(clicks) )

	dev_num <- as.integer(dev.cur()-1L)
	if (use_jquery[1] == TRUE) {
  	add_click_event(dn = dev_num, id = as.integer( ids ), str = clicks )
	} else {
  	add_click_event_raw(dn = dev_num, id = as.integer( ids ), str = clicks )
	}

	invisible()
}

#' @title add id
#'
#' @description add id to graphical elements.
#' @param ids integer vector of graphical elements identifiers (returned by
#' \code{\link{rvg_tracer_off}}).
#' @param data_id user id to be associated with \code{ids}.
#' @param use_jquery logical indicating whether to rely on jQuery or not (default: "\code{TRUE}")
#'
#' @export
set_data_id = function( ids, data_id, use_jquery=TRUE){
	stopifnot( .Device == "dsvg_device" )
	if( is.factor(data_id) )
		data_id = as.character( data_id )

	stopifnot( is.character(data_id) )
	stopifnot( is.numeric(ids) )
	stopifnot( length(ids) == length(data_id) )

	dev_num <- as.integer(dev.cur()-1L)
	if (use_jquery[1] == TRUE) {
  	add_data_id(dn = dev_num, id = as.integer( ids ), data_id = data_id )
	} else {
  	add_data_id_raw(dn = dev_num, id = as.integer( ids ), data_id = data_id )
	}

	invisible()
}

#' @importFrom utils zip
#' @importFrom R.utils getAbsolutePath
pack_folder <- function( folder, target ){
  target <- getAbsolutePath(target, expandTilde = TRUE)
  curr_wd <- getwd()
  zip_dir <- folder
  setwd(zip_dir)
  zip(zipfile = target, flags = "-r9Xq",
      files = list.files(all.files = TRUE, recursive = TRUE, include.dirs = FALSE))
  setwd(curr_wd)
  target
}

read_relationship <- function(filename) {
  doc <- read_xml( x = filename )
  children <- xml_children( doc )
  ns <- xml_ns( doc )
  id <- sapply( children, xml_attr, attr = "Id", ns)
  int_id <- as.integer( gsub(pattern = "^rId", replacement = "", x = id ) )
  type <- sapply( children, xml_attr, attr = "Type", ns)
  target <- sapply( children, xml_attr, attr = "Target", ns)
  out <- list( data = data.frame(id = id, int_id = int_id, type = type, target = target, stringsAsFactors = FALSE ) )
  out$max_int <- max(int_id, na.rm = T)
  out
}

"%||%" <- function(a, b) {
  if (!is.null(a)) a else b
}

