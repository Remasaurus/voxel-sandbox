{
  description = "Epic Voxel Sandbox built in C++";

  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
  };

  outputs = {nixpkgs, ...} @ inputs: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
    libs = with pkgs; [
      cmake
      libGL
      git
      pkg-config

      wayland
      wayland-scanner
      wayland-protocols
      libxkbcommon

      libx11
      libxcursor
      libxrandr
      libxi
      libxinerama
      libGL

      glfw
      glm
      imgui
    ];
  in {
    devShells.${system}.default = pkgs.mkShell {
      packages = libs;
      shellHook = ''
        export LD_LIBRARY_PATH="${pkgs.lib.makeLibraryPath libs}:$LD_LIBRARY_PATH"
        echo "Development shell started"
      '';
    };
  };
}
