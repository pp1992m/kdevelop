add_definitions(-DTRANSLATION_DOMAIN=\"kdevplatform\")

add_subdirectory(examples)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

set(sublime_LIB_SRCS
    area.cpp
    areaindex.cpp
    container.cpp
    controller.cpp
    document.cpp
    mainwindow.cpp
    mainwindow_p.cpp
    mainwindowoperator.cpp
    urldocument.cpp
    tooldocument.cpp
    view.cpp
    sublimedefs.cpp
    aggregatemodel.cpp
    holdupdates.cpp

    idealcontroller.cpp
    ideallayout.cpp
    idealtoolbutton.cpp
    idealdockwidget.cpp
    idealbuttonbarwidget.cpp
)
ecm_qt_declare_logging_category(sublime_LIB_SRCS
    HEADER debug.h
    IDENTIFIER SUBLIME
    CATEGORY_NAME "kdevplatform.sublime"
)
kdevplatform_add_library(KDevPlatformSublime SOURCES ${sublime_LIB_SRCS})
target_link_libraries(KDevPlatformSublime
PUBLIC
    KF5::Parts
PRIVATE
    KF5::KIOWidgets
)

install(FILES
    area.h
    areaindex.h
    areawalkers.h
    container.h
    controller.h
    document.h
    mainwindow.h
    mainwindowoperator.h
    urldocument.h
    sublimedefs.h
    tooldocument.h
    view.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/kdevplatform/sublime COMPONENT Devel)
