--
-- meteo system database schema
--
-- (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
--
-- $Id: meteo.sql,v 1.2 2002/11/18 02:36:42 afm Exp $
--
create table meteo.stationdata (
	-- the time stamp is the key
	timekey			integer not null,
	station			char(8) not null,
	-- add broken up time
	year 			smallint not null,
	month 			tinyint not null,
	mday			tinyint not null,
	hour			tinyint not null,
	min			tinyint	not null,
	-- add the data
	temperature		real,
	temperature_inside	real,
	barometer		real,
	humidity		tinyint,
	humidity_inside		tinyint,
	rain			real,
	raintotal		real,
	windspeed		real,
	winddir			real,
	windgust		real,
	windx			real,
	windy			real,
	duration		double,
	solar			real,
	uv			real,
	samples			int,
	-- grouping attributes
	group300		int not null,
	group1800		int not null,
	group7200		int not null,
	group86400		int not null,
	-- the timestamp is the primary key
	primary key (timekey, station)
);

create table meteo.averages (
	-- identification
	timekey			integer not null,
	station			char(8) not null,
	-- interval duration
	intval		int not null,
	-- 
	temperature		real,
	temperature_max		real,
	temperature_min		real,
	temperature_inside	real,
	temperature_inside_max	real,
	temperature_inside_min	real,
	barometer		real,
	barometer_max		real,
	barometer_min		real,
	humidity		tinyint,
	humidity_max		tinyint,
	humidity_min		tinyint,
	humidity_inside		tinyint,
	humidity_inside_max	tinyint,
	humidity_inside_min	tinyint,
	rain			real,
	windspeed		real,
	winddir			real,
	windgust		real,
	solar			real,
	uv			real,
	--
	primary key (intval, station, timekey)
);
