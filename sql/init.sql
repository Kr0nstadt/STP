BEGIN;

CREATE TABLE IF NOT EXISTS worlds (
    id integer GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    name text NOT NULL CHECK (octet_length(name) BETWEEN 1 AND 80)
);

CREATE TABLE IF NOT EXISTS blocks (
    world_id integer NOT NULL REFERENCES worlds(id) ON DELETE CASCADE,
    x integer NOT NULL CHECK (x BETWEEN 0 AND 11),
    y integer NOT NULL CHECK (y BETWEEN 0 AND 7),
    material text NOT NULL CHECK (material IN ('dirt', 'stone', 'wood')),
    PRIMARY KEY (world_id, x, y)
);

COMMIT;

