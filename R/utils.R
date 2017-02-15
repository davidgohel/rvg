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


#' @title set attributes to graphical elements
#'
#' @description set attributes with javascript instructions
#' to graphical elements.
#' @param ids integer vector of graphical elements identifiers (returned by
#' \code{\link{rvg_tracer_off}}).
#' @param attribute name of the attribute to set.
#' @param str values to set for the attribute.
#'
#' @export
set_attr = function( ids, attribute, str ){
  stopifnot( .Device == "dsvg_device" )
  if( is.factor(str) )
    str = as.character( str )
  if( is.factor(attribute) )
    str = as.character( attribute )

  stopifnot( is.character(attribute) )
  stopifnot( is.character(str) )
  stopifnot( is.numeric(ids) )
  stopifnot( length(ids) == length(str) )
  stopifnot( length(attribute) == 1 )

  if( any( grepl(pattern = "'", str) ) )
    stop("str cannot contain single quote \"'\".")
  dev_num <- as.integer(dev.cur()-1L)
  add_attribute(dn = dev_num,
               id = as.integer( ids ),
               str = str,
               name = attribute
               )

  invisible()
}


#' @importFrom xml2 read_xml xml_children xml_ns xml_attr
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

