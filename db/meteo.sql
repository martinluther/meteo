--
-- meteo system database schema
--
-- (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
--
-- $Id: meteo.sql,v 1.24 2006/05/07 19:47:22 afm Exp $
--
create table if not exists station (
	name		varchar(60) not null,
	id		tinyint not null,
	timezone	varchar(12),	-- time zone
	offset		int not null,
	longitude	double not null,	-- degrees, + east, - west
	latitude	double not null,	-- degress, + north, - south
	altitude	double not null,	-- altitude sea level, meters!
	primary key(id)
);
-- Altendorf:	a WeatherMonitor II
insert into station(name, id, timezone, offset, longitude, latitude, altitude)
	values ('Altendorf', 0, 'MET', 3600, 47.18, 8.81, 610);
-- Gurin: a VantagePro Plus, recently converted to wireless operation
insert into station(name, id, timezone, offset, longitude, latitude, altitude)
	values ('Gurin', 1, 'MET', 3600, 46.3, 8.5, 2000);
-- Lajeado: a VantagePro Plus with soil moisture and soil temperature sensor
insert into station(name, id, timezone, offset, longitude, latitude, altitude)
	values ('Lajeado', 2, 'MET', -10800, -29.46693, -51.96138, 166);
-- Frauenfeld: a cabled VantagePro Plus 
insert into station(name, id, timezone, offset, longitude, latitude, altitude)
	values ('Frauenfeld', 3, 'MET', 3600, 8.9, 47.55, 477);

create table if not exists sensor (
	name		varchar(60) not null,
	id		tinyint not null,
	stationid	tinyint not null,
	primary key(id)
);
-- the WeatherMonitor II has inside (console) and outside sensors
insert into sensor (name, id, stationid) values ('inside', 0, 0);
insert into sensor (name, id, stationid) values ('outside', 1, 0);
-- the VantagePro Plus usually has the console (inside values, pressure)
-- and the ISS
insert into sensor (name, id, stationid) values ('console', 2, 1);
insert into sensor (name, id, stationid) values ('iss', 3, 1);
-- the VantagePro Plus of Univates has a soil station in addition to console
-- and iss
insert into sensor (name, id, stationid) values ('console', 4, 2);
insert into sensor (name, id, stationid) values ('iss', 5, 2);
insert into sensor (name, id, stationid) values ('soil1', 6, 2);
-- Frauenfeld is again an ordinary VantagePro Plus with only console and iss
insert into sensor (name, id, stationid) values ('console', 7, 3);
insert into sensor (name, id, stationid) values ('iss', 8, 3);

drop table if exists mfield;
create table mfield (
	name	varchar(60) not null,
	id	tinyint not null,
	unit	varchar(32) not null,
	class	varchar(32),
	primary key(name)
);

insert into mfield(name, id, unit, class)
	values('temperature',		0, 'C', 'TemperatureValue');
insert into mfield(name, id, unit, class)
	values('temperature_min',	1, 'C', 'TemperatureValue');
insert into mfield(name, id, unit, class)
	values('temperature_max',	2, 'C', 'TemperatureValue');

insert into mfield(name, id, unit, class)
	values('humidity',		10, '%', 'HumidityValue');
insert into mfield(name, id, unit, class)
	values('humidity_max',		11, '%', 'HumidityValue');
insert into mfield(name, id, unit, class)
	values('humidity_min',		12, '%', 'HumidityValue');

insert into mfield(name, id, unit, class)
	values('wetness',	20, 'index', 'LeafwetnessValue');
insert into mfield(name, id, unit, class)
	values('wetness_min',	21, 'index', 'LeafwetnessValue');
insert into mfield(name, id, unit, class)
	values('wetness_max',	22, 'index', 'LeafwetnessValue');

insert into mfield(name, id, unit, class)
	values('barometer',	30, 'hPa', 'PressureValue');
insert into mfield(name, id, unit, class)
	values('barometer_min',	31, 'hPa', 'PressureValue');
insert into mfield(name, id, unit, class)
	values('barometer_max',	32, 'hPa', 'PressureValue');

insert into mfield(name, id, unit, class)
	values('solar',		40, 'W/m2', 'SolarValue');

insert into mfield(name, id, unit, class)
	values('uv',		41, 'index', 'UVValue');

insert into mfield(name, id, unit, class)
	values('rain',			50, 'mm', 'Rain');
insert into mfield(name, id, unit, class)
	values('raintotal',		51, 'mm', 'Rain');
insert into mfield(name, id, unit, class)
	values('rainrate',		52, 'mm/h', 'RainRate');

-- Normally every data value needs a value class that knows how to perform
-- conversions. However, wind is somewhat special as it is a vector, not
-- a simple scalar. This problem is solved by the Wind class, which is attached
-- to the wind field (id 60). As a side effect, this class also maintains
-- the winddir, windx and windy fields (the latter to are only use to speed up
-- accumulations). Therefore it is not an error that winddir, windx and windy
-- don't have a class associated with it.
insert into mfield(name, id, unit, class)
	values('wind',		60, 'm/s', 'Wind');
insert into mfield(name, id, unit)
	values('winddir',	61, 'deg');
insert into mfield(name, id, unit, class)
	values('windgust',	62, 'm/s', 'WindSpeed');
insert into mfield(name, id, unit)
	values('windx',		63, 'm/s');
insert into mfield(name, id, unit)
	values('windy',		64, 'm/s');

insert into mfield(name, id, unit, class)
	values('moisture',	70, 'cb', 'MoistureValue');

insert into mfield(name, id, unit, class)
	values('waterlevel',	71, 'm', 'LevelValue');
insert into mfield(name, id, unit, class)
	values('waterlevel_min',72, 'm', 'LevelValue');
insert into mfield(name, id, unit, class)
	values('waterlevel_max',73, 'm', 'LevelValue');

insert into mfield(name, id, unit, class)
	values('sunrise',	100, 'unix', 'TimeValue');
insert into mfield(name, id, unit, class)
	values('sunset',	101, 'unix', 'TimeValue');

insert into mfield(name, id, unit, class)
	values('battery',	110, 'V', 'VoltageValue');
insert into mfield(name, id, unit, class)
	values('transmitter',	111, 'V', 'TransmitterStatus');

insert into mfield(name, id, unit)
	values('duration',	120, 's');

insert into mfield(name, id, unit)
	values('samples',	121, '');

create table if not exists sdata (
	timekey		integer not null,	-- unix time of beginning of
						-- interval where this value
						-- was measured
	sensorid	tinyint not null,
	fieldid		tinyint not null,
	value		float not null,
	primary key (timekey, sensorid, fieldid)
);
-- the following index i necessary to make the meteodbdump program perform
create index sdatax1 on sdata(sensorid, timekey);

create table if not exists avg (
	timekey		integer not null,	-- timekey, i.e. unixtime
						-- + offset
	intval		integer not null,
	sensorid	tinyint not null,
	fieldid		tinyint not null,
	value		float not null,
	primary key (timekey, intval, sensorid, fieldid)
);
-- the following index is necessary for good performance with large datasets,
-- see the README for the reason
create index avgx1 on avg(intval, timekey);

