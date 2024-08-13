# Webcam_LIBRARY - the name of the libraries to link against

SET (Webcam_LIBRARY_SEARCH_DIR
  /usr
  /usr/local
  /opt
  /opt/local
  )

find_library (Webcam_LIBRARY
  NAMES webcam
  PATH_SUFFIXES lib64 lib
  PATHS ${Webcam_LIBRARY_SEARCH_DIR}
  )
