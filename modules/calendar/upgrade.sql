CREATE TABLE schedule_notes (
  id int(10) PRIMARY KEY NOT NULL,
  title varchar(50) NOT NULL,
  description text NOT NULL,
  entrydate date NOT NULL,
  owner varchar(50) NOT NULL,
  schedule_id int(10) NOT NULL
);