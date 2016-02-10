INSTALL(FILES exampleModule.cpp DESTINATION ${datadir}/examples)
INSTALL(FILES pxlexample.cc DESTINATION ${datadir}/examples)
INSTALL(FILES pypxlexample.py DESTINATION ${datadir}/examples)
INSTALL(FILES pxlmerge.py DESTINATION ${datadir}/examples)
INSTALL(FILES pxlexampleMakefile DESTINATION ${datadir}/examples RENAME Makefile)

include_directories (
	${PXL_SOURCE_DIR}/modules/include
	${PXL_SOURCE_DIR}/scripting/include
	${PXL_SOURCE_DIR}/algorithms/include
	${PXL_SOURCE_DIR}/core/include
	${PXL_SOURCE_DIR}/hep/include
	${PXL_SOURCE_DIR}/astro/include
	${PXL_SOURCE_DIR}/xml/include
	${PXL_SOURCE_DIR}/lib
	${CMAKE_BINARY_DIR}/include
)

add_library(exampleModule SHARED MODULE
	exampleModule.cpp
)
target_link_libraries (exampleModule pxl-core pxl-hep pxl-modules)