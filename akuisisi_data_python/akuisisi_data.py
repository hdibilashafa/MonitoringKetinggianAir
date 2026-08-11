import serial
import time
import openpyxl
import re

ser = serial.Serial('COM5', 9600, timeout=1)
time.sleep(2)

# Buat Excel
wb = openpyxl.Workbook()
ws = wb.active
ws.title = "Data Tangki Air"
ws.append([
    "Waktu",
    "Tinggi Air Sensor 1 (cm)", "Status Sensor 1",
    "Tinggi Air Sensor 2 (cm)", "Status Sensor 2"
])

print("✅ Merekam data Sensor 1 & Sensor 2...")
print("Tekan Ctrl + C untuk berhenti\n")

try:
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if not line:
            continue

        waktu = time.strftime('%Y-%m-%d %H:%M:%S')

        # Default
        tinggi1 = tinggi2 = ""
        status1 = status2 = ""

        # ===== SENSOR 1 =====
        m1 = re.search(r"Sensor1 Filter=(\d+)cm", line)
        if m1:
            tinggi1 = m1.group(1)

        if "Sensor1 Filter=ERR" in line:
            status1 = "ERROR"
        else:
            # Parse status khusus untuk Sensor 1
            status_match1 = re.search(
                r"Sensor1 Filter=\d+cm\s*(AIR PENUH|AIR TINGGI|AIR SEDANG|AIR RENDAH)", line
            )
            if status_match1:
                status1 = status_match1.group(1)
            elif "AIR PENUH" in line:
                status1 = "AIR PENUH"
            elif "AIR TINGGI" in line:
                status1 = "AIR TINGGI"
            elif "AIR SEDANG" in line:
                status1 = "AIR SEDANG"
            elif "AIR RENDAH" in line:
                status1 = "AIR RENDAH"

        # ===== SENSOR 2 =====
        m2 = re.search(r"Sensor2 Raw=(\d+)cm", line)
        if m2:
            tinggi2 = m2.group(1)

        if "Sensor2 Raw=ERR" in line:
            status2 = "ERROR"
        else:
            # Parse status khusus untuk Sensor 2
            status_match2 = re.search(
                r"Sensor2 Raw=\d+cm\s*(AIR PENUH|AIR TINGGI|AIR SEDANG|AIR RENDAH)", line
            )
            if status_match2:
                status2 = status_match2.group(1)
            elif "AIR PENUH" in line:  # Fallback jika format tidak pas
                status2 = "AIR PENUH"
            elif "AIR TINGGI" in line:
                status2 = "AIR TINGGI"
            elif "AIR SEDANG" in line:
                status2 = "AIR SEDANG"
            elif "AIR RENDAH" in line:
                status2 = "AIR RENDAH"

        # Simpan ke Excel
        ws.append([waktu, tinggi1, status1, tinggi2, status2])
        print(f"{waktu} | Sensor1 Filter = {tinggi1} cm \"{status1}\" | "
              f"Sensor2 Raw = {tinggi2} cm \"{status2}\"\n")

        time.sleep(0.5)

except KeyboardInterrupt:
    filename = f"Data_Tangki_Air_{time.strftime('%Y%m%d_%H%M')}.xlsx"
    wb.save(filename)
    ser.close()
    print(f"\n✅ Data tersimpan: {filename}")
