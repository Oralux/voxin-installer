# $LOG expected to be set

SPD_CONF_SYSTEM="/home/*/.config/speech-dispatcher/speechd.conf /etc/speech-dispatcher/speechd.conf"
SPD_CONF_USER="$HOME/.config/speech-dispatcher/speechd.conf"

SPD_VOXIN_CONF_USER="${SPD_CONF_USER%speechd.conf}modules/voxin.conf"

spd_install_local_conf() {
    local SPD_VOXIN_CONF_SYSTEM="$TOPDIR"/opt/oralux/voxin/share/speech-dispatcher/voxin.conf
    if [ -e "$SPD_VOXIN_CONF_SYSTEM" ] && [ ! -e "$SPD_VOXIN_CONF_USER" ] && [ ! -h "$SPD_VOXIN_CONF_USER" ]; then
	ln -s "$SPD_VOXIN_CONF_SYSTEM" "$SPD_VOXIN_CONF_USER"
    fi
}

spd_uninstall_local_conf() {
    [ -h "$SPD_VOXIN_CONF_USER" ] && unlink "$SPD_VOXIN_CONF_USER"
}

getList() {
    if [ "$SYSTEM_WIDE_INSTALL" = 1 ]; then
	LIST="$SPD_CONF_SYSTEM"
    else
	LIST="$SPD_CONF_USER"
    fi
}

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
    getList
    ls -t $LIST 2>>"$LOG" | while read file; do		
	local newModule=$(grep "^[[:space:]]*#[[:space:]]*voxinadded \(.*\)" "$file" | awk '{print $2}')
	isAdded "$file" "$module" && removeModule "$file" "$module"		
    done
}

function spd_conf_set() {
    local module="$1"
    local file
    getList
    ls -t $LIST 2>>"$LOG" | while read file; do
		isAnyAdded "$file"
		if [ "$?" = 0 ]; then
			isAdded "$file" "$module" || addModule "$file" "$module"
		fi
    done
}

# Check if the voxin module must be added to the speechd.conf file
#
# Voxin must be added if there are uncommented AddModule lines in
# speechd.conf and none of them concerns voxin.
# Checked with speech-dispatcher 0.9.1 (Gentoo)
#
# return 0 if voxin must be added
function spd_conf_is_update_required() {
    local file
    getList
    files=$(ls -t $LIST 2>>"$LOG")
    while read file; do
	local i
	local voxin_added=0
	local added="$(getAdded "$file")"
	[ -z "$added" ] && continue
	for i in $added; do
	    [ "$i" = "voxin" ] && voxin_added=1 && break
	done
	[ $voxin_added = 0 ] && { echo "update file: $file (added module: $added)" >> "$LOG"; return 0; }
    done <<< "$files"

    return 1
}

# Install voxin.conf if not yet provided
function spd_conf_install_voxin_conf() {
    local src=$1
    local new_voxin_conf="$TOPDIR"/opt/oralux/voxin/share/speech-dispatcher/voxin.conf
    [ -h "$new_voxin_conf" ] && unlink "$new_voxin_conf"
    [ ! -e "$new_voxin_conf" ] && ln -s "$src" "$new_voxin_conf"
}
