#' @useDynLib rvg,.registration = TRUE
#' @importFrom Rcpp sourceCpp
#' @import systemfonts

r_font_families <- c("sans", "serif", "mono", "symbol")

#' @importFrom gdtools font_family_exists
default_fontname <- function() {
  def_fonts <- if (Sys.info()["sysname"] == "Windows") {
    c(
      sans = "Arial",
      serif = "Times New Roman",
      mono = "Courier New",
      symbol = "Symbol"
    )
  } else if (Sys.info()["sysname"] == "Darwin") {
    c(
      sans = "Helvetica",
      serif = "Times",
      mono = "Courier",
      symbol = "Symbol"
    )
  } else {
    c(
      sans = "DejaVu Sans",
      serif = "DejaVu serif",
      mono = "DejaVu mono",
      symbol = "DejaVu Sans"
    )
  }

  bool_family_exists <- sapply(def_fonts, font_family_exists)
  def_fonts[!bool_family_exists] <- lapply(
    names(def_fonts)[!bool_family_exists],
    match_family
  )
  def_fonts
}


#' @importFrom gdtools match_family
validate_fonts <- function(system_fonts = list()) {
  system_fonts <- system_fonts[unlist(lapply(system_fonts, font_family_exists))]
  missing_fonts <- setdiff(r_font_families, names(system_fonts))
  system_fonts[missing_fonts] <- default_fontname()[missing_fonts]
  system_fonts
}
