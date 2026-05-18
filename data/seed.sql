INSERT OR IGNORE INTO buses(bus_number, model, total_mileage, photo_path) VALUES
  (101, 'MAN Lion''s Coach', 152000, 'docs/assets/bus-101.png'),
  (205, 'Mercedes Tourismo', 221500, 'docs/assets/bus-205.png'),
  (312, 'Volvo 9700', 98000, 'docs/assets/bus-312.png');

INSERT OR IGNORE INTO routes(name, start_point, end_point, distance_km) VALUES
  ('Минск - Мир - Несвиж', 'Минск', 'Несвиж', 260),
  ('Минск - Брестская крепость', 'Минск', 'Брест', 710),
  ('Полоцкое кольцо', 'Минск', 'Полоцк', 470),
  ('Нарочанский край', 'Минск', 'Нарочь', 330);

INSERT OR IGNORE INTO crew_members(last_name, personnel_number, experience_years, category, address, birth_year, bus_number) VALUES
  ('Иванов', 'T-1001', 12, 'водитель', 'Минск, ул. Ленина, 10', 1984, 101),
  ('Петров', 'T-1002', 8, 'экскурсовод', 'Минск, ул. Сурганова, 5', 1990, 101),
  ('Сидорова', 'T-1003', 6, 'механик', 'Минск, пр. Победителей, 21', 1992, 101),
  ('Козлов', 'T-2001', 15, 'водитель', 'Брест, ул. Московская, 7', 1979, 205),
  ('Орлова', 'T-2002', 9, 'экскурсовод', 'Брест, ул. Гоголя, 12', 1988, 205),
  ('Мельник', 'T-2003', 5, 'механик', 'Брест, ул. Советская, 9', 1994, 205),
  ('Смирнов', 'T-3001', 10, 'водитель', 'Витебск, ул. Кирова, 4', 1986, 312),
  ('Волкова', 'T-3002', 7, 'экскурсовод', 'Витебск, ул. Лазо, 17', 1991, 312),
  ('Егоров', 'T-3003', 4, 'механик', 'Витебск, ул. Правды, 3', 1996, 312);

INSERT OR IGNORE INTO users(username, password, role, crew_member_id) VALUES
  ('admin', 'admin', 'admin', NULL),
  ('ivanov', 'crew123', 'crew', 1),
  ('kozlova', 'crew123', 'crew', 5),
  ('smirnov', 'crew123', 'crew', 7);

INSERT OR IGNORE INTO trips(bus_number, departure_date, arrival_date, route_id, passengers, ticket_price) VALUES
  (101, '2026-04-01', '2026-04-01', 1, 38, 85.0),
  (101, '2026-04-05', '2026-04-06', 4, 31, 110.0),
  (205, '2026-04-03', '2026-04-04', 2, 44, 160.0),
  (205, '2026-04-11', '2026-04-12', 3, 41, 140.0),
  (312, '2026-04-08', '2026-04-08', 1, 29, 90.0),
  (312, '2026-04-15', '2026-04-15', 4, 35, 120.0);

INSERT OR IGNORE INTO media_files(entity_type, entity_id, file_path, description) VALUES
  ('bus', 101, 'docs/assets/bus-101.png', 'Изображение автобуса 101'),
  ('bus', 205, 'docs/assets/bus-205.png', 'Изображение автобуса 205');
