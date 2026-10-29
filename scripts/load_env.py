Import("env")

from pathlib import Path


def read_dotenv(path):
    values = {}
    if not path.exists():
        return values

    for line_number, raw_line in enumerate(
        path.read_text(encoding="utf-8").splitlines(), start=1
    ):
        line = raw_line.strip()
        if not line or line.startswith("#"):
            continue
        if "=" not in line:
            raise ValueError(".env line {} must use KEY=VALUE".format(line_number))
        key, value = line.split("=", 1)
        key = key.strip()
        value = value.strip()
        if not key:
            raise ValueError(".env line {} has an empty key".format(line_number))
        if len(value) >= 2 and value[0] == value[-1] and value[0] in ("'", '"'):
            value = value[1:-1]
        values[key] = value
    return values


project_dir = Path(env.subst("$PROJECT_DIR"))
dotenv = read_dotenv(project_dir / ".env")
required = (
    "WIFI_SSID",
    "WIFI_PASSWORD",
    "FIREBASE_API_KEY",
    "FIREBASE_PROJECT_ID",
    "DEVICE_ID",
)
missing = [key for key in required if not dotenv.get(key)]
if missing:
    raise ValueError("Missing values in .env: {}".format(", ".join(missing)))


for key in required:
    env.Append(CPPDEFINES=[(key, env.StringifyMacro(dotenv[key]))])
