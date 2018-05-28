DROP TABLE IF EXISTS sms_payment;
DROP TABLE IF EXISTS outbound_call_payment;
DROP TABLE IF EXISTS incoming_call_payment;
DROP TABLE IF EXISTS rate_region;
DROP TABLE IF EXISTS user_connected_rate;
DROP TABLE IF EXISTS user_connected_service;
DROP TABLE IF EXISTS package_state;
DROP TABLE IF EXISTS regions;
DROP TABLE IF EXISTS services;
DROP TABLE IF EXISTS rates;
DROP TABLE IF EXISTS users;

CREATE TABLE users (
	id serial NOT NULL,
	score FLOAT NOT NULL,
	phone varchar(12) NOT NULL,
	PRIMARY KEY (id)
);

CREATE TABLE package_state (
	user_id integer NOT NULL,
	count_sec_into_net integer NOT NULL,
	count_sec_out_net integer NOT NULL,
	count_sms integer NOT NULL,
	count_mb integer NOT NULL,
	PRIMARY KEY (user_id),
    FOREIGN KEY (user_id) REFERENCES users (id)
);

CREATE TABLE rates (
	id serial NOT NULL,
	into_net_price integer NOT NULL,
	out_net_price integer NOT NULL,
	sms_price integer NOT NULL,
	count_mb integer NOT NULL,
	count_sms integer NOT NULL,
	count_sec_into_net integer NOT NULL,
	count_sec_out_net integer NOT NULL,
        period integer NOT NULL,
        cost FLOAT NOT NULL,
	about TEXT NOT NULL,
	PRIMARY KEY (id)
);

CREATE TABLE services (
	id serial NOT NULL,
	name TEXT NOT NULL,
	count_sec_into_net integer NOT NULL,
	count_sec_out_net integer NOT NULL,
	count_sms integer NOT NULL,
	count_mb integer NOT NULL,
	live_time integer NOT NULL,
	cost FLOAT NOT NULL,
	about TEXT NOT NULL,
	PRIMARY KEY (id)
);

CREATE TABLE user_connected_rate (
	user_id integer NOT NULL,
	rate_id integer NOT NULL,
	date TIMESTAMP NOT NULL,
	PRIMARY KEY (user_id, rate_id, date),
    FOREIGN KEY (user_id) REFERENCES users (id),
	FOREIGN KEY (rate_id) REFERENCES rates (id)
);

CREATE TABLE user_connected_service (
	user_id integer NOT NULL,
	service_id integer NOT NULL,
	date TIMESTAMP NOT NULL,
	PRIMARY KEY (user_id, service_id, date),
    FOREIGN KEY (user_id) REFERENCES users (id),
	FOREIGN KEY (service_id) REFERENCES services (id)
);

CREATE TABLE regions (
	id serial NOT NULL,
	name TEXT NOT NULL,
	PRIMARY KEY (id)
);

CREATE TABLE rate_region (
	rate_id integer NOT NULL,
	region_id integer NOT NULL,
	PRIMARY KEY (rate_id, region_id),
    FOREIGN KEY (rate_id) REFERENCES rates (id),
	FOREIGN KEY (region_id) REFERENCES regions (id)
);

CREATE TABLE incoming_call_payment (
	user_id_to INT NOT NULL,
	number_from varchar(12) NOT NULL,
	date TIMESTAMP NOT NULL,
	paid FLOAT NOT NULL,
	count_sec INT NOT NULL,
	PRIMARY KEY (user_id_to, number_from, date),
	FOREIGN KEY (user_id_to) REFERENCES users (id)
);

CREATE TABLE outbound_call_payment (
	user_id_from INT NOT NULL,
	number_to varchar(12) NOT NULL,
	date TIMESTAMP NOT NULL,
	paid FLOAT NOT NULL,
	count_sec INT NOT NULL,
	PRIMARY KEY (user_id_from, number_to, date),
	FOREIGN KEY (user_id_from) REFERENCES users (id)
);

CREATE TABLE sms_payment (
	user_id INT NOT NULL,
	paid FLOAT NOT NULL,
	date TIMESTAMP NOT NULL,
	PRIMARY KEY (user_id, date),
	FOREIGN KEY (user_id) REFERENCES users (id)
);
