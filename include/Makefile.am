#
# Makefile.am -- generate a portable Makefile for the meteo interface package
#
# (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
#
# $Id: Makefile.am,v 1.39 2004/02/27 22:09:04 afm Exp $
#

noinst_HEADERS =							\
	MeteoTypes.h MeteoException.h Vector.h				\
	Value.h BasicValue.h VoltageValue.h TransmitterStatus.h		\
	TemperatureValue.h HumidityValue.h PressureValue.h 		\
	SolarValue.h UVValue.h MoistureValue.h LeafwetnessValue.h 	\
	LevelValue.h WindSpeed.h Wind.h Rain.h RainRate.h TimeValue.h	\
	BasicValueFactory.h ValueFactory.h				\
	Recorder.h BasicRecorder.h MinmaxRecorder.h VoltageRecorder.h	\
	TemperatureRecorder.h HumidityRecorder.h PressureRecorder.h	\
	SolarRecorder.h UVRecorder.h MoistureRecorder.h TimeRecorder.h	\
	LeafwetnessRecorder.h LevelRecorder.h WindRecorder.h		\
	RainRecorder.h RainRateRecorder.h TransmitterRecorder.h		\
	LastRecorder.h							\
	RecorderFactory.h DataRecorder.h 				\
	Configuration.h Field.h Field_internals.h StationInfo.h		\
	FQField.h FQField_internals.h					\
	Timeval.h Channel.h SerialChannel.h TcpChannel.h FileChannel.h	\
	ChannelFactory.h						\
	SensorStationInfo.h SensorStation.h				\
	Station.h DavisStation.h OldDavisStation.h 			\
	WMII.h GroWeather.h Energy.h Health.h				\
	VantagePro.h ReaderInfo.h StationFactory.h			\
	Converter.h BasicConverter.h ConverterFactory.h TimeConverter.h	\
	TemperatureConverter.h HumidityConverter.h RainConverter.h	\
	PressureConverter.h SolarConverter.h UVConverter.h		\
	MoistureConverter.h LeafwetnessConverter.h LevelConverter.h	\
	WindConverter.h RainRateConverter.h VoltageConverter.h		\
	TransmitterConverter.h Calibrator.h				\
	PacketReader.h BasicPacketReader.h				\
	BytePacketReader.h ShortPacketReader.h NibblePacketReader.h	\
	TemperatureReader.h OldTemperatureReader.h 			\
	ExtraTemperatureReader.h BarometerReader.h UVReader.h 		\
	WMIIWindReader.h VantageProWindReader.h OldWindReader.h		\
	RainReader.h RainRateReader.h TimeReader.h VoltageReader.h	\
	PacketReaderFactory.h						\
	Datasink.h Pidfile.h Level.h Timelabel.h			\
	Derivedvalue.h Dewpoint.h Heatindex.h Windchill.h		\
	BasicQueryResult.h Query.h QueryResult.h QueryProcessor.h	\
	Tdata.h Dataset.h						\
	Point.h Dimension.h Rectangle.h Color.h Label.h Frame.h		\
	Scale.h Axis.h GraphPoint.h MapArea.h ImageMap.h GraphWindow.h	\
	Graphics.h SunData.h						\
	Datarecord.h Averager.h						\
	MsgCommon.h MsgQueuer.h MsgDequeuer.h				\
	Timestamp.h							\
	Daemon.h							\
	ChunkDumper.h							\
	crc.h 								\
	printver.h mdebug.h

VERSION_FILE = meteo_vers.h

EXTRA_DIST = $(VERSION_FILE) README

all-local:	$(VERSION_FILE)

meteo_vers.h:	../configure.in
	../shtool version -l c -s @VERSION@ -n Meteo -p meteo_ $(VERSION_FILE)

