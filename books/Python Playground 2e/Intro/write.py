import sqlite3

con = sqlite3.connect('test.db')
cur = con.cursor()
cur.execute("CREATE TABLE sensor_data (TS datetime, ID text, VAL numeric)")
for i in range(10):
    cur.execute(
        "INSERT into sensor_data VALUES (datetime('now'),'ABC', ?)", (i, ))
con.commit()
con.close()
exit()
