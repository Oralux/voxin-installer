# $LOG expected to be set

LIST="/home/*/.config/speech-dispatcher/speechd.conf /etc/speech-dispatcher/speechd.conf /usr/share/speech-dispatcher/conf/speechd.conf"

getDefault() {
    local filename="$1"
	[ ! -e "$filename" ] && return
	grep -E '^[[:space:]]*DefaultModule' "$filename" | cut -f2 -d" "
}

getAdded() {
    local filename="$1"
	[ ! -e "$filename" ] && return
	grep -E '^[[:space:]]*AddModule[[:space:]]+' "$filename" | cut -f2 -d'"'
}

isAdded() {
    local filename="$1"
    local module="$2"
	[ ! -e "$filename" ] && return
    grep -Eq '^[[:space:]]*AddModule[[:space:]]+"'"$module"'"[[:space:]]+"sd_'"$module"'"[[:space:]]+"'"$module"'.conf"[[:space:]]*' "$filename"
}

isAnyAdded() {
    local filename="$1"
	[ ! -e "$filename" ] && return
    grep -Eq '^[[:space:]]*AddModule[[:space:]]+' "$filename"
}

addModule() {
    local filename="$1"
    local module="$2"
	[ ! -e "$filename" ] && return
    sed -i -e '/^[[:space:]]*AddModule[[:space:]]/b ins' -e b -e ':ins' -e 'a\'$'\n''AddModule "'$module'"        "sd_'$module'"     "'$module'.conf"' -e ': done' -e 'n;b done' "$filename"
}

removeModule() {
    local filename="$1"
    local module="$2"
	[ ! -e "$filename" ] && return
    sed -i -r '/^.*AddModule[[:space:]]+"'$module'"[[:space:]]+.*/d' "$filename"
}


isDefaultModule() {
    local filename="$1"
    local module="$2"
	[ ! -e "$filename" ] && return
    grep -Eq '^[[:space:]]*DefaultModule[[:space:]]+'"$module"'[[:space:]]*' "$filename"
}

# setDefaultModule() {
#     local filename="$1"
#     local newModule="$2"    
# 	local oldModule=$(grep "^[[:space:]]*DefaultModule\(.*\)" "$filename" | awk '{print $2}')
# 	if [ -n "$oldModule" ]; then
# 		sed -i "/^[[:space:]]*#[[:space:]]*voxinadded/d" "$filename"
# 		sed -i -e "\$a#voxinadded $oldModule" -e 's/^[[:space:]]*DefaultModule.*/DefaultModule '"$newModule"'/' "$filename"
# 	else
# 		sed -i 's/^[[:space:]]*DefaultModule.*/DefaultModule '"$newModule"'/' "$filename"
# 	fi
# }

function spd_conf_unset() {
    local module="$1"
    ls -t $LIST 2>>"$LOG" | while read file; do		
		local newModule=$(grep "^[[:space:]]*#[[:space:]]*voxinadded \(.*\)" "$file" | awk '{print $2}')
		isAdded "$file" "$module" && removeModule "$file" "$module"		
	done
}

function spd_conf_set() {
    local module="$1"
    local file
	
    ls -t $LIST 2>>"$LOG" | while read file; do
		isAnyAdded "$file"
		if [ "$?" = 0 ]; then
			isAdded "$file" "$module" || addModule "$file" "$module"
		fi
    done
}

# Check if the voxin must be added to the speechd.conf file
# Condition implemented: speechd.conf has at least one module added to
# the speechd.conf file and distinct from default or voxin module
function spd_conf_is_update_required() {
	local filename="$1"
	[ ! -e "$filename" ] && return
	local default="$(getDefault "$filename")"
	local added="$(getAdded "$filename")"
	[ -z "$added" ] && return
	local count=0
	local i
	for i in $added; do
		[ "$i" != "$default" ] && [ "$i" != "voxin" ] && count=$((count+1))
	done
	[ "$count" -gt 0 ]		
}

