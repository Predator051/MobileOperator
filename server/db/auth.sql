DROP TABLE IF EXISTS server_users;

CREATE TABLE server_users (
    user_id BIGSERIAL UNIQUE NOT NULL,
    user_login text UNIQUE NOT NULL,
    password text NOT NULL,
    creation_time timestamp DEFAULT NOW(),
    role int NOT NULL,
    salt text NOT NULL,
    PRIMARY KEY(user_login)
);

CREATE TABLE server_session (
    session text NOT NULL,
    user_id BIGINT NOT NULL,
    PRIMARY KEY(session),
    FOREIGN KEY(user_id) REFERENCES server_users (user_id)
);
