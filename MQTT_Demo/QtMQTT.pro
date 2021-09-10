greaterThan(QT_MAJOR_VERSION, 4) {
    load(qt_parts)
} else {
    TEMPLATE = subdirs

    SUBDIRS = src mqtt

    mqtt.depends = src
}
