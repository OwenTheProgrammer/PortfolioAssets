import bpy
from bpy import context
from bpy_extras.io_utils import ExportHelper
import struct

bl_info = {
    "name": "Unity Object Exporter",
    "description": "A tool that exports to .bin for my game",
    "author": "OwenTheProgrammer",
    "version": (1,0,0),
    "blender": (2,93,1),
    "category": "Import-Export"
}

positions = []
#vertOffsets = []

#push all the data to the file from every selected object
def WriteBinFile(ctx, path, selected, collection):
    #write vertex positions for each triangle
    for m in bpy.context.selected_objects:
        loc = m.location
        positions.append(loc[0])
        positions.append(loc[1])
        positions.append(loc[2])
        #vertOffsets.append(offset)
        #offset += len(m.data.vertices)
    with open(path, "wb") as file:
        #write uvs:
        file.write(bytearray([0x76, 0x65, 0x72, 0x74, 0x3A]))
        file.write(struct.pack("i", len(positions) * 4))
        for d in positions:
            file.write(struct.pack("f", d))
        #write triangel offsets:
        #file.write(bytearray([0x6F, 0x66, 0x73, 0x3A]))
        #file.write(struct.pack("i", len(vertOffsets) / 3))
        #for d in vertOffsets:
        #    file.write(struct.pack("i", d))
        #close the file
        file.flush()
        file.close()
    print(f"Wrote to {path}")
    return {'FINISHED'}

#export button in operator panel
class OBJECT_OT_export_button(bpy.types.Operator):
    bl_idname = "object.export_button"
    bl_label = "Bin Operator"

    def execute(self, ctx):
        bpy.ops.exporter.export_queue('INVOKE_DEFAULT')
        return {'FINISHED'}
#blender panel add-on
class OBJECT_PT_export_panel(bpy.types.Panel):
    bl_idname = "object.export_panel"
    bl_label = "Mesh Export"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "Exporter"
    bl_options = {'DEFAULT_CLOSED'}

    def draw(self, context):
        layout = self.layout
        layout.operator(OBJECT_OT_export_button.bl_idname, text="Export", icon="PACKAGE")

#"write to file" window attributes
class ExportQueue(bpy.types.Operator, ExportHelper):
    bl_idname = "exporter.export_queue"
    bl_label = "Export Binary Data"
    filename_ext = ".bin"

    filter_glob: bpy.props.StringProperty(
        default="*.bin",
        options={'HIDDEN'},
        maxlen=255
    )
    sel_only: bpy.props.BoolProperty(
        name="Selection Only",
        description="Export Mesh Data from selected objects",
        default=True
    )
    col_only: bpy.props.BoolProperty(
        name="In Collection Only",
        description="Only export from the currently active collection",
        default=False
    )

    def execute(self, ctx):
        return WriteBinFile(ctx, self.filepath, self.sel_only, self.col_only)

def menu_func_export(self, ctx):
    self.layout.operator(ExportQueue.bl_idname, text="Exporter")

#register all add-on features
def register():
    bpy.utils.register_class(OBJECT_PT_export_panel)
    bpy.utils.register_class(OBJECT_OT_export_button)
    bpy.utils.register_class(ExportQueue)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

#remove all add-on features
def unregister():
    bpy.utils.unregister_class(OBJECT_PT_export_panel)
    bpy.utils.unregister_class(OBJECT_OT_export_button)
    bpy.utils.unregister_class(ExportQueue)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

#debug
if __name__ == "__main__":
    register()
    bpy.ops.exporter.export_queue('INVOKE_DEFAULT')