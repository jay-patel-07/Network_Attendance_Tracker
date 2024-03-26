# Network_Attendance_Tracker

Network Attendance Tracker is a basic client-server system for attendance management over a network. The client portion allows users to input their personal details such as name, class, and roll number. After establishing a connection to the server, it sends this information, waits for a brief period, and then marks the user as "present" in the attendance record maintained by the server. The server side listens for incoming connections, receives attendance data from clients, parses and stores it in a CSV file. 
