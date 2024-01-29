#!/usr/bin/env bash
# Version: 3.0
# Date: 2023-11-06
# This bash script generates MDK-Middleware documentation
#
# Pre-requisites:
# - bash shell (for Windows: install git for Windows)
# - doxygen 1.9.2

set -o pipefail

# Set version of gen pack library
# For available versions see https://github.com/Open-CMSIS-Pack/gen-pack/tags.
# Use the tag name without the prefix "v", e.g., 0.7.0
REQUIRED_GEN_PACK_LIB="0.9.1"

DIRNAME=$(dirname "$(readlink -f "$0")")
REQ_DXY_VERSION="1.9.2"

RUN_LINKCHECKER=1
COMPONENTS=()

function usage() {
  echo "Usage: $(basename "$0") [-h] [-s] [-c <comp>]"
  echo " -h,--help               Show usage"
  echo " -s,--no-linkcheck       Skip linkcheck"
  echo " -c,--component <comp>   Select component <comp> to generate documentation for. "
  echo "                         Can be given multiple times. Defaults to all components."
}

while [[ $# -gt 0 ]]; do
  case $1 in
    '-h'|'help')
      usage
      exit 1
    ;;
    '-s'|'--no-linkcheck')
      RUN_LINKCHECKER=0
    ;;
    '-c'|'--component')
      shift
      COMPONENTS+=("$1")
    ;;
    *)
      echo "Invalid command line argument: $1" >&2
      usage
      exit 1
    ;;
  esac
  shift # past argument
done

############ DO NOT EDIT BELOW ###########

# Set GEN_PACK_LIB_PATH to use a specific gen-pack library root
# ... instead of bootstrap based on REQUIRED_GEN_PACK_LIB
if [[ -f "${GEN_PACK_LIB_PATH}/gen-pack" ]]; then
  . "${GEN_PACK_LIB_PATH}/gen-pack"
else
  . <(curl -sL "https://raw.githubusercontent.com/Open-CMSIS-Pack/gen-pack/main/bootstrap")
fi

find_git
find_doxygen "${REQ_DXY_VERSION}"
[[ ${RUN_LINKCHECKER} != 0 ]] && find_linkchecker

pushd "${DIRNAME}" > /dev/null || exit 1

echo_log "Generating documentation ..."

# USB documentation: prepare input files before they are feed to doxygen
#
# This function replaces '%Instance%' with 'n' in all USB templates
#
doc_preprocess_usb() {
  # Set USB templates location
  local source_dir=$(realpath "../../../USB/Template")

  # Loop through files that start with USBD
  for file in "$source_dir"/USBD*; do
    if [ -f "$file" ]; then
      # Extract the filename without the path
      filename=$(basename "$file")

      # Use sed to replace '%Instance%' with 'n' and save to the current directory
      sed "s/%Instance%/n/g" "$file" > "src/$filename"

      #echo "Processed: $(realpath "src/$filename")"
    fi
  done
}

# Remove input files previously prepared in preprocessing
#
doc_postprocess_usb() {
  # Set USB templates location
  local source_dir=$(realpath "../../../USB/Template")

  # Loop through files that start with USBD
  for file in "$source_dir"/USBD*; do
    if [ -f "$file" ]; then
      # Extract the filename without the path
      filename=$(basename "$file")

      # Remove file with the same name from the current directory
      rm -f "src/$filename"

      #echo "Removed: $(realpath "src/$filename")"
    fi
  done
}

function generate() {
  if [[ ! (${#COMPONENTS[@]} == 0 || ${COMPONENTS[*]} =~ $1) ]]; then
    return
  fi

  pushd "$1" > /dev/null || exit 1
  
  if [[ "$1" == "USB" ]]; then
    doc_preprocess_usb
  fi

  echo_log "\"${UTILITY_DOXYGEN}\" \"$1.dxy\""
  "${UTILITY_DOXYGEN}" "$1.dxy"

  if [[ "$1" == "USB" ]]; then
    doc_postprocess_usb
  fi

  popd > /dev/null || exit 1
}

generate "General"
generate "FileSystem"
generate "Network"
generate "USB"

#[[ ${RUN_LINKCHECKER} != 0 ]] && check_links --timeout 120 "${DIRNAME}/../html/index.html" "${DIRNAME}"

popd > /dev/null || exit 1

exit 0
