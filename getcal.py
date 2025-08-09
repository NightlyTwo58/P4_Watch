from ics import Calendar
import requests
import json
from datetime import datetime, time
from zoneinfo import ZoneInfo

ICS_URL = "https://p168-caldav.icloud.com/published/2/MjAxNzI0Mzc2ODQyMDE3Mhes2tRTG0mTstBV6AMgWn-YAorMpukIz6l1whwjzNVRIG-eunFfvfvCYV9FtcgDSQi49PbPtC88wjho0eZRaKk"

def ics_to_json():
    c = Calendar(requests.get(ICS_URL).text)

    tz = ZoneInfo("America/Los_Angeles")

    now = datetime.now(tz)
    end_of_day = datetime.combine(now.date(), time(23, 59, 59), tzinfo=tz)

    with open('time.txt', 'w') as f:
        f.write(now.strftime("%x") + '\n')
        f.write(now.strftime("%X") + '\n')
        f.write(now.strftime("%A") + '\n')


    events = []
    print(len(c.events))
    for event in c.events:
        event_start = event.begin.astimezone(tz)
        if now <= event_start <= end_of_day:
            start_time = event_start.isoformat()
            events.append({
                "title": event.name,
                "start": start_time[len(start_time) - 5:]
            })

    with open("calendar.json", "w") as f:
        json.dump(events, f, indent=2)
    with open('calendar.txt', 'w') as f:
        for event in events:
            f.write(event["title"] + ", " + event["start"] + "," + '\n')

if __name__ == "__main__":
    ics_to_json()
