#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.41 2004/05/08 21:08:55 afm Exp $
#

lib_LTLIBRARIES = libmeteo.la

METEOCONFFILE = \"@METEOCONFFILE@\"
AM_CPPFLAGS = -DMETEOCONFFILE=$(METEOCONFFILE) @LIBXML_CFLAGS@

VERSION_FILE = ../include/meteo_vers.h

libmeteo_la_LDFLAGS = -version-info `../shtool version -d libtool $(VERSION_FILE)`

libmeteo_la_SOURCES =							\
	MeteoException.cc Vector.cc					\
	BasicValue.cc Value.cc VoltageValue.cc TransmitterStatus.cc	\
	TemperatureValue.cc HumidityValue.cc PressureValue.cc		\
	SolarValue.cc UVValue.cc MoistureValue.cc LeafwetnessValue.cc	\
	LevelValue.cc WindSpeed.cc Wind.cc Rain.cc RainRate.cc		\
	TimeValue.cc							\
	BasicValueFactory.cc ValueFactory.cc				\
	BasicRecorder.cc Recorder.cc MinmaxRecorder.cc 			\
	TemperatureRecorder.cc HumidityRecorder.cc PressureRecorder.cc	\
	SolarRecorder.cc UVRecorder.cc MoistureRecorder.cc 		\
	LeafwetnessRecorder.cc LevelRecorder.cc VoltageRecorder.cc	\
	WindRecorder.cc RainRecorder.cc RainRateRecorder.cc		\
	RecorderFactory.cc TransmitterRecorder.cc TimeRecorder.cc	\
	DataRecorder.cc LastRecorder.cc					\
	Converter.cc BasicConverter.cc ConverterFactory.cc		\
	TemperatureConverter.cc HumidityConverter.cc 			\
	PressureConverter.cc SolarConverter.cc UVConverter.cc		\
	RainConverter.cc RainRateConverter.cc TimeConverter.cc		\
	MoistureConverter.cc LeafwetnessConverter.cc LevelConverter.cc	\
	WindConverter.cc VoltageConverter.cc TransmitterConverter.cc	\
	Configuration.cc Field_internals.cc Field.cc StationInfo.cc	\
	FQField.cc FQField_internals.cc					\
	Timeval.cc 							\
	Channel.cc SerialChannel.cc TcpChannel.cc FileChannel.cc	\
	ChannelFactory.cc						\
	SensorStationInfo.cc SensorStation.cc				\
	Station.cc DavisStation.cc OldDavisStation.cc WMII.cc 		\
	GroWeather.cc Energy.cc	Health.cc VantagePro.cc			\
	ReaderInfo.cc StationFactory.cc					\
	Calibrator.cc							\
	PacketReader.cc BasicPacketReader.cc				\
	BytePacketReader.cc ShortPacketReader.cc NibblePacketReader.cc	\
	TemperatureReader.cc OldTemperatureReader.cc			\
	ExtraTemperatureReader.cc BarometerReader.cc UVReader.cc	\
	WMIIWindReader.cc VantageProWindReader.cc OldWindReader.cc	\
	RainReader.cc RainRateReader.cc TimeReader.cc VoltageReader.cc	\
	PacketReaderFactory.cc						\
	Level.cc Timelabel.cc 						\
	Datasink.cc Pidfile.cc Dataset.cc				\
	Derivedvalue.cc Dewpoint.cc Heatindex.cc Windchill.cc		\
	Query.cc QueryResult.cc QueryProcessor.cc			\
	Tdata.cc 							\
	Point.cc Dimension.cc Rectangle.cc Color.cc Label.cc Frame.cc	\
	Scale.cc Axis.cc GraphPoint.cc MapArea.cc ImageMap.cc 		\
	SunData.cc Sun.cc GraphWindow.cc Graphics.cc			\
	Datarecord.cc							\
	Averager.cc							\
	MsgCommon.cc MsgQueuer.cc MsgDequeuer.cc			\
	Timestamp.cc							\
	crc.c 								\
	Daemon.cc							\
	ChunkDumper.cc							\
	Lock.cc								\
	printver.c							\
	mdebug.c  

EXTRA_DIST = README README.sunriset
