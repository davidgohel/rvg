#' Make A DrawingML plotting object to print in Powerpoint
#'
#' Only Powerpoint outputs currently supported. Pandoc >=2.4 is required.
#' Currently plots with rasters are not supported.
#'
#' @param code plotting instructions
#' @param ggobj ggplot object to print. argument code will be ignored if this
#'   argument is supplied.
#' @param layout either a character indicating layout type: "default",
#'   "left_col", "right_col", "full_slide", or a list with elements "width",
#'   "height", "offx", and "offy" in inches. Layouts are drawn from the
#'   powerpoint template used for the R Markdown document.
#' @param bg,fonts,pointsize,editable Parameters passed to \code{\link{dml_pptx}}
#' @export
dml <- function(code, ggobj = NULL, layout = "default",
                bg = "white", fonts = list(), pointsize = 12, editable = TRUE) {
  out <- list()
  out$code <- substitute(code)
  out$ggobj <- ggobj
  if (!(layout %in% c("default", "left_col", "right_col", "full_slide") ||
        (inherits(layout, "list") &&
         setequal(names(layout), c("width", "height", "offx", "offy"))))) {
    stop("Invalid layout value")
  }
  out$layout <- layout
  out$bg <- bg
  out$fonts <- fonts
  out$pointsize <- pointsize
  out$editable <- editable
  class(out) <- "dml"
  return(out)
}

#' @title Render a plot as a Powerpint DrawingML object
#' @description Function used to render DrawingML in knitr/rmarkdown documents.
#' Only Powerpoint outputs currently supported
#'
#' @param x a \code{dml} object
#' @param ... further arguments, not used.
#' @author Noam Ross
#' @importFrom knitr knit_print asis_output opts_knit opts_current
#' @importFrom rmarkdown pandoc_version
#' @importFrom xml2 xml_find_first
#' @export
knit_print.dml <- function(x, ...) {
  if (pandoc_version() < 2.4) {
    stop("pandoc version >= 2.4 required for DrawingML output in pptx")
  }

  if (is.null(opts_knit$get("rmarkdown.pandoc.to")) ||
      opts_knit$get("rmarkdown.pandoc.to") != "pptx") {
    stop("DrawingML currently only supported for pptx output")
  }

  if (inherits(x$layout, "list")) {
    id_xfrm <- x$layout
  } else {
    id_xfrm <- get_content_layout(x$layout)
    id_xfrm <- lapply(id_xfrm, function(x) x / 914400 )
  }

  dml_file <- tempfile(fileext = ".dml")

  dml_pptx(file = dml_file, width = id_xfrm$width, height = id_xfrm$height,
           offx = id_xfrm$offx, offy = id_xfrm$offy, pointsize = x$pointsize,
           editable = x$editable, standalone = FALSE)

  tryCatch({
    if (!is.null(x$ggobj) ) {
      stopifnot(inherits(x$ggobj, "ggplot"))
      print(x$ggobj)
    } else {
      eval(x$code)
    }
  }, finally = dev.off() )

  dml_xml <- read_xml(dml_file)

  rast_element <- xml_find_first(dml_xml, "//p:pic")
  if (!inherits(rast_element, "xml_missing"))
    stop("Graphics with rasters not yet supported")

  dml_str <- paste(
    as.character(xml_find_first(dml_xml, "//p:grpSp")),
    collapse = "\n"
  )

  knit_print(asis_output(
    paste("```{=openxml}", dml_str, "```", sep = "\n")
  ))
}



#' If size is not provided, get the size of the main content area of the slide
#' @noRd
#' @importFrom officer read_pptx
get_content_layout_uncached <- function(layout) {
  ref_pptx <- read_pptx(get_reference_pptx())
  if (layout == "full_slide") {
    return(get_slide_size(ref_pptx))
  } else {
    layout_data <- ref_pptx$slideLayouts$get_xfrm_data()
    if (layout == "default") {
      id_xfrm <- subset(layout_data, master_file == "slideMaster1.xml" & type == "body" &
                          file == "slideLayout2.xml")
    } else {
      id_xfrm <- subset(layout_data, master_file == "slideMaster1.xml" & type == "body" &
                          file == "slideLayout4.xml")
      if (layout == "left_col") {
        id_xfrm <- id_xfrm[id_xfrm$offx == min(id_xfrm$offx), ]
      } else if (layout == "right_col") {
        id_xfrm <- id_xfrm[id_xfrm$offx == max(id_xfrm$offx), ]
      } else {
        stop("Unknown layout type")
      }
    }
      id_xfrm <- as.list(id_xfrm[c("cx", "cy", "offx", "offy")])
      names(id_xfrm) <- c("width", "height", "offx", "offy")
      return(id_xfrm)
  }
}

#' @importFrom memoise memoise
#' @noRd
get_content_layout <- memoise(get_content_layout_uncached)


#' Get slide size
#' @noRd
#' @importFrom xml2 xml_attrs xml_find_first
get_slide_size <- function(ref_pptx) {
  pres <- ref_pptx$presentation$get()
  id_xfrm <- xml_attrs(xml2::xml_find_first(pres, "p:sldSz"))
  id_xfrm <- as.list(as.integer(id_xfrm[c("cx", "cy")]))
  names(id_xfrm) <- c("width", "height")
  id_xfrm$offx <- 0L
  id_xfrm$offy <- 0L
  id_xfrm
}

#' Get the document being used as a template
#' @noRd
#' @importFrom knitr opts_knit
get_reference_pptx_uncached <- function(reference_pptx = NULL) {
  if (!is.null(reference_pptx)) return(reference_pptx)

  pandoc_args <- knitr::opts_knit$get("rmarkdown.pandoc.args")

  rd <- grep("--reference-doc", pandoc_args)
  if (length(rd)) {
    reference_pptx <- pandoc_args[rd + 1]
  } else {
    reference_pptx <- get_default_pandoc_pptx()
  }
  return(reference_pptx)
}

#' @noRd
#' @importFrom knitr opts_knit
#' @importFrom memoise memoise
get_reference_pptx <- memoise(get_reference_pptx_uncached)

#' Print the default pptx template from pandoc, which may be defined in the
#' user data directory
#' Need to use the same pandoc as rmarkdown/rstudio, be cross-platform
#' @noRd
#' @importFrom rmarkdown pandoc_exec
get_default_pandoc_pptx <- function(outfile = tempfile(fileext = ".pptx")) {
  system2(pandoc_exec(),
          args = c("--print-default-data-file", "reference.pptx"),
          stdout = outfile
  )
  return(outfile)
}

#' @export
#' @rdname dml
print.dml <- function(dml) {
  cat("DrawingML object with parameters:\n")
  cat(paste0("  ", names(dml), ": ", as.character(dml), collapse = "\n"))
}
