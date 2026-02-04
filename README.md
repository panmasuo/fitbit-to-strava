# Fitbit activities converter

Fitbit application cannot sync the non-GPS workouts/activities
to Strava application. Developers seems to not work on this issue.

There are some tools online for syncing those two apps, but connecting
private accounts to them seems dangerous.

## Roadmap

### 1.0

- [x] Fitbit API access
- [x] Fitbit authentication
- [x] Fitbit pull activities
- [x] Parse multiple time-series data (heart-rate, etc.) into TCX file
- [x] Strava API access
- [x] Strava push created TCX file
- [x] Configure from file
- [x] Create user instructions
- [x] Clean CLI session

### 2.0

- [ ] Simple GUI application
- [x] Dockerize

## Application building

Conan package manager and C++23 is used in this project, make sure they are
installed:

```bash
# debian
sudo apt install build-essential g++-14 cmake python3-pip python3-venv

# break system packages or use venv - as you wish
pip install conan --break-system-packages
```

```bash
# arch
sudo pacman -S base-devel cmake python-pip

# break system packages or use venv - as you wish
pip install conan --break-system-packages
```

Then, if conan is not initialized, do it:

```bash
conan profile detect --force
```

then compile:

```bash
export CXX=g++-14

conan install . --output-folder=build --build=missing
cmake --preset conan-release
cmake --build --preset conan-release
```

## Application running

Fill Fitbit and Strava client secrets in `client.json` file, built the application
and then run it:

```bash
./build/fitbit
```

If ran first time, app will ask you to click on the link, accept the
authentication request and paste back the URL that was provided in the
browser bar. (two times for each app)

After that, last Fitbit activities will be created in `workouts/` directory
and app will ask you to pick the workout from the list - to post it to Strava.

All done, if you wish to post another activity, run the script again, but
now, the authentication stage will be skipped.

Or build and run using Docker:

```bash
docker build -t fitbit .
docker run --rm -it fitbit
```

## Fitbit API access

[Start here](https://dev.fitbit.com/apps/new) - login and register an application.
For redirect URL provide something similar to `https://localhost:5000`.


## Strava API access

[Start here](https://developers.strava.com/docs/getting-started/#account) -
follow the instructions to login and create the app. When filling the fields
where URL are needed use for example `https://localhost:5000`.
