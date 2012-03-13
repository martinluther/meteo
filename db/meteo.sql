--
-- meteo system database schema
--
-- (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
--
-- $Id: meteo.sql,v 1.17 2004/02/24 23:33:20 afm Exp $
--
create table if not exists station (
	name		varchar(60) not null,
	id		tinyint not null,
	timezone	varchar(12),	-- time zone
	primary key(name)
);
insert into station(name, id, timezone) values ('Altendorf', 0, 'MET');
insert into station(name, id, timezone) values ('Gurin', 1, 'MET');
insert into station(name, id, timezone) values ('Lajeado', 2, 'MET');
insert into station(name, id, timezone) values ('Frauenfeld', 2, 'MET');

create table if not exists sensor (
	name		varchar(60) not null,
	id		tinyint not null,
	stationid	tinyint not null,
	primary key(id)
);
insert into sensor (name, id, stationid) values ('inside', 0, 0);
insert into sensor (name, id, stationid) values ('outside', 1, 0);
insert into sensor (name, id, stationid) values ('console', 2, 1);
insert into sensor (name, id, stationid) values ('iss', 3, 1);
insert into sensor (name, id, stationid) values ('console', 4, 2);
insert into sensor (name, id, stationid) values ('iss', 5, 2);
insert into sensor (name, id, stationid) values ('soil1', 6, 2);
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
	values('wetness',		20, 'index', 'LeafwetnessValue');
insert into mfield(name, id, unit, class)
	values('wetness_min',		21, 'index', 'LeafwetnessValue');
insert into mfield(name, id, unit, class)
	values('wetness_max',		22, 'index', 'LeafwetnessValue');

insert into mfield(name, id, unit, class)
	values('barometer',		30, 'hPa', 'PressureValue');
insert into mfield(name, id, unit, class)
	values('barometer_min',		31, 'hPa', 'PressureValue');
insert into mfield(name, id, unit, class)
	values('barometer_max',		32, 'hPa', 'PressureValue');

insert into mfield(name, id, unit, class)
	values('solar',			40, 'W/m2', 'SolarValue');

insert into mfield(name, id, unit, class)
	values('uv',			41, 'index', 'UVValue');

insert into mfield(name, id, unit, class)
	values('rain',			50, 'mm', 'Rain');
insert into mfield(name, id, unit, class)
	values('raintotal',		51, 'mm', 'Rain');
insert into mfield(name, id, unit, class)
	values('rainrate',		52, 'mm/h', 'RainRate');

insert into mfield(name, id, unit, class)
	values('wind',			60, 'm/s', 'Wind');
insert into mfield(name, id, unit)
	values('winddir',		61, 'deg');
insert into mfield(name, id, unit, class)
	values('windgust',		62, 'm/s', 'WindSpeed');
insert into mfield(name, id, unit)
	values('windx',			63, 'm/s');
insert into mfield(name, id, unit)
	values('windy',			64, 'm/s');

insert into mfield(name, id, unit, class)
	values('moisture',		70, 'cb', 'MoistureValue');

insert into mfield(name, id, unit, class)
	values('waterlevel',		71, 'm', 'LevelValue');
insert into mfield(name, id, unit, class)
	values('waterlevel_min',	72, 'm', 'LevelValue');
insert into mfield(name, id, unit, class)
	values('waterlevel_max',	73, 'm', 'LevelValue');

insert into mfield(name, id, unit, class)
	values('battery',		110, 'V', 'VoltageValue');
insert into mfield(name, id, unit, class)
	values('transmitter',		111, 'V', 'TransmitterStatus');

insert into mfield(name, id, unit)
	values('duration',		120, 's');

insert into mfield(name, id, unit)
	values('samples',		121, '');

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

