#!/usr/bin/env bash

if [[ "$(uname)" == "Darwin" ]]; then
  if ! type gfind >/dev/null 2>&1; then
    echo "Error: GNU find requires. Run 'brew install findutils' to install." >&2
    exit 1
  fi
  shopt -s expand_aliases
  alias find=gfind
  alias head=ghead
  alias tail=gtail
fi

LINES=($(grep -n 'list of versions' version.js | cut -d: -f1))
VERSIONS_BEGIN=$(expr ${LINES[0]} - 1)
VERSIONS_END=$(expr ${LINES[1]} + 1)

get_versions() {
  local folders=($(find -maxdepth 1  -mindepth 1 -type d) $(find -maxdepth 1 -mindepth 1 -type l))
  declare -A versions
  for folder in "${folders[@]#./}"; do
    if [ -f "${folder}/${1}/footer.js" ]; then
      version=$(grep "Version" "${folder}/${1}/footer.js" | \
                sed -r "s/.*'Version ([^']+)'.*/\1/" | \
                sed -r "s/\+.*$//" | \
                sed -r "s/([^0-9])$/\10/")
      versions["$version"]="$folder"
    fi
  done
  fields=()
  for version in $(IFS=$'\n'; sed -r 's/$/_/' <<< "${!versions[*]}" | sort -Vr | sed -r 's/_$//'); do
      fields+=( "\"${versions[$version]}\": \"$version\"" )
  done
  
  echo "    \"$1\": {"
  echo -n "        "
  echo "$(IFS=,; echo "${fields[*]}")" | sed 's/,/,\n        /g'
  echo -n "    }"
}

mv version.js version.js.bak

head -n "${VERSIONS_BEGIN}" version.js.bak > version.js
echo "//--- list of versions‚ ---" >> version.js
echo "const versions = {" >> version.js

get_versions "General" >> version.js
echo "," >> version.js

get_versions "FileSystem" >> version.js
echo "," >> version.js

get_versions "Network" >> version.js
echo "," >> version.js

get_versions "USB" >> version.js
echo "," >> version.js

echo "}" >> version.js
echo "//--- list of versions ---" >> version.js
tail -n "+${VERSIONS_END}" version.js.bak >> version.js

rm version.js.bak

exit 0
